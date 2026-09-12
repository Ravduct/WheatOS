nasm -f bin mbr.asm -o mbr.bin
nasm -f bin stage2.asm -o stage2.bin -l stage2.lst
nasm -f bin kernel-entry.asm -o kernel.bin

rm -f disk.img
rm -f disk.vdi

cat mbr.bin stage2.bin kernel.bin > disk.img

qemu-system-x86_64 -drive format=raw,file=disk.img