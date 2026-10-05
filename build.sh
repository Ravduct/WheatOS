mkdir -p build

nasm -f bin mbr.asm -o build/mbr.bin
nasm -f bin stage2.asm -o build/stage2.bin -l build/stage2.lst
nasm -f elf64 kernel_gdt.asm -o build/kernel_gdt.o
truncate -s 16896 build/stage2.bin

nasm -f elf64 kernel-entry.asm -o build/kernel-entry.o
gcc -ffreestanding -fno-stack-protector -nostdlib -mno-red-zone -c kernel.c -o build/kernel.o
ld -T linker.ld build/kernel-entry.o build/kernel.o build/kernel_gdt.o -o build/kernel.elf
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

qemu-system-x86_64 -drive format=raw,file=build/disk.img -m 4G -no-shutdown -d int -D build/qemu.log