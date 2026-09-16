nasm -f bin mbr.asm -o build/mbr.bin
nasm -f bin stage2.asm -o build/stage2.bin -l build/stage2.lst
truncate -s 16896 build/stage2.bin

nasm -f elf64 kernel-entry.asm -o build/kernel-entry.o
x86_64-elf-gcc -ffreestanding -c kernel.c -o build/kernel.o
x86_64-elf-ld -T linker.ld build/kernel-entry.o build/kernel.o -o build/kernel.elf
x86_64-elf-objcopy -O binary build/kernel.elf build/kernel.bin

rm -f build/disk.img
rm -f build/disk.vdi

cat build/mbr.bin build/stage2.bin build/kernel.bin > build/disk.img

qemu-system-x86_64 -drive format=raw,file=build/disk.img -no-shutdown -d int -D build/qemu.log