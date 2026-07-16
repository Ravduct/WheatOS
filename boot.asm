[org 0x7C00]
[bits 16]

_start:
    KERNEL_OFFSET equ 0x1000
    mov [boot_drive], dl

    mov bp, 0x9000
    mov sp, bp

    call load_kernel
    call switch_to_32bit

    jmp $

%include "disk.asm"
%include "gdt.asm"
%include "switch-to-32bit.asm"

[bits 16]
load_kernel:
    mov bx, KERNEL_OFFSET
    mov dh, 2
    mov dl, [boot_drive]
    call disk_load
    ret

[bits 32]
begin_32bit:
    call KERNEL_OFFSET
    jmp $

boot_drive db 0

times 510-($-$$) db 0
db 0x55, 0xAA