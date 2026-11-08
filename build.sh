mkdir -p build

nasm -f bin mbr.asm -o build/mbr.bin
nasm -f bin stage2.asm -o build/stage2.bin -l build/stage2.lst
nasm -f elf64 asm_lib/kernel_gdt.asm -o build/kernel_gdt.o
truncate -s 16896 build/stage2.bin

nasm -f elf64 asm_lib/kernel-entry.asm -o build/kernel-entry.o

gcc -ffreestanding -fno-stack-protector -nostdlib -mno-red-zone -c kernel.c -o build/kernel.o
gcc -ffreestanding -fno-stack-protector -nostdlib -mno-red-zone -fPIE \
    -fno-asynchronous-unwind-tables -fno-unwind-tables -c shell/shell.c -o build/shell.o
nasm -f elf64 asm_lib/user_entry.asm -o build/user-entry.o
ld -T user.ld build/user-entry.o build/shell.o -o build/user_shell.elf
objcopy -O binary build/user_shell.elf build/user_shell.bin

user_shell_size=$(stat -c %s build/user_shell.bin)
if [ "$user_shell_size" -gt 2097152 ]; then
    echo "User shell image exceeds the 2 MiB code page: $user_shell_size bytes" >&2
    exit 1
fi

nasm -f elf64 asm_lib/user_prog.asm -o build/user_prog.o

ld -T linker.ld build/kernel-entry.o build/kernel.o build/kernel_gdt.o build/user_prog.o -o build/kernel.elf
objcopy -O binary build/kernel.elf build/kernel.bin

kernel_size=$(stat -c %s build/kernel.bin)
sector_count=$(( (kernel_size + 511) / 512 ))

mov_addr_hex=$(grep "mov word \[sector_count\], " build/stage2.lst | awk '{print $2}')
mov_addr_dec=$((16#$mov_addr_hex))
imm_offset=$((mov_addr_dec + 4))

sector_count_hex=$(printf '%04x' "$sector_count")
low_byte=${sector_count_hex:2:2}
high_byte=${sector_count_hex:0:2}

printf "\\x$low_byte\\x$high_byte" | dd of=build/stage2.bin bs=1 seek=$imm_offset count=2 conv=notrunc status=none

echo "Kernel size: $kernel_size bytes, Sector count patched to: $sector_count"

rm -f build/disk.img
rm -f build/disk.vdi

cat build/mbr.bin build/stage2.bin build/kernel.bin > build/disk.img

[ -f build/fs.img ] || truncate -s 1M build/fs.img
qemu-system-x86_64 -drive format=raw,file=build/disk.img -drive format=raw,file=build/fs.img,if=ide,index=1 -m 4G -no-shutdown -D build/qemu.log