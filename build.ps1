& NASM -f bin boot.asm -o boot.bin
& NASM -f bin stage2.asm -o stage2.bin

del disk.img -ErrorAction SilentlyContinue
del disk.vdi -ErrorAction SilentlyContinue

cmd /c copy /b boot.bin+stage2.bin disk.img

& "C:\Program Files\Oracle\VirtualBox\VBoxManage.exe" convertfromraw disk.img disk.vdi --format VDI

& qemu-system-x86_64.exe -drive format=raw,file=disk.img