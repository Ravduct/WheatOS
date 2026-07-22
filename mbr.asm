[org 0x7C00]
[bits 16]

global boot_drive
%include "disk.asm"
%include "print.asm"

_start:
    ; registers and stack setup
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    mov si, boot_msg
    call print_string

    ;save boot drive number
    mov [boot_drive], dl

    ;normalize cs register
    jmp 0x0000:continue
    continue:
    call check_int13h
    
    ;disk load
    mov word [sector_count], 4
    mov word [my_offset], 0x8000
    mov word [my_segment], 0x0000
    mov dword [lba_low], 1

    mov dl, [boot_drive]
    call disk_load
    mov si, stage1_ready_msg
    call print_string
    jmp 0x0000:0x8000

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
.error:
    mov si, error_msg
    call print_string
    jmp $

boot_drive db 0
error_msg db "stage 1 error", 0
stage1_ready_msg db "stage 1 ready", 0
boot_msg db "booting...", 0

times 510-($-$$) db 0
db 0x55, 0xAA