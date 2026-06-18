& "C:\Program Files\NASM\nasm.exe" -f bin boot.asm -o boot.bin


del disk.img -ErrorAction SilentlyContinue
del disk.vdi -ErrorAction SilentlyContinue

$disk= New-Object byte[] (1474560)
[IO.File]::WriteAllBytes("disk.img", $disk)

$boot= [IO.File]::ReadAllBytes("boot.bin")
$disk= [IO.File]::ReadAllBytes("disk.img")
[Array]::Copy($boot, 0, $disk, 0, 512)
[IO.File]::WriteAllBytes("disk.img", $disk)

& "C:\Program Files\Oracle\VirtualBox\VBoxManage.exe" convertfromraw disk.img disk.vdi --format VDI