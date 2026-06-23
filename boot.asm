[org 0x7C00]
[bits 16]

_start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    sti

    mov [boot_drive], dl

    mov si, switching_stage2_message
    call print_loop

    jmp hang

print_loop:
    lodsb
    cmp al, 0
    je .done
    mov ah, 0x0E
    int 0x10
    jmp print_loop

.done: ret

hang:
    hlt
    jmp hang

switching_stage2_message: db 'Loading into Stage 2 bootloader', 0
boot_drive: db 0

times 510-($-$$) db 0
db 0x55, 0xAA