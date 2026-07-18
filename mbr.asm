[org 0x7C00]
[bits 16]

_start:
    ; registers and stack setup
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    ;save boot drive number
    mov [boot_drive], dl

    ;normalize cs register
    jmp 0x0000:continue
    continue:
    call check_int13h
    
    ;not yet
    call load_kernel
    call switch_to_32bit

    jmp $

%include "disk.asm"
%include "gdt.asm"
%include "switch-to-32bit.asm"

[bits 16]
check_int13h:
    mov ah, 0x41
    mov bx, 0x55AA
    mov dl, [boot_drive]
    int 0x13
    jc .error
    cmp bx, 0xAA55
    jne .error
    ret
load_kernel:
    mov bx, KERNEL_OFFSET
    mov dh, 2
    mov dl, [boot_drive]
    call disk_load
    ret

.error:
    jmp $

[bits 32]
begin_32bit:
    call KERNEL_OFFSET
    jmp $

boot_drive db 0

times 510-($-$$) db 0
db 0x55, 0xAA