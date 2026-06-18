[org 0x7C00]
[bits 16]

_start:
    mov si, switching_32bit_message
    call print_loop
    cli
    call enable_A20_gate

    jump hang


enable_A20_gate:
    mov ax, 0x2401
    int 0x15

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
    jump hang

switching_32bit_message: db 'Loading into Protected Mode 32-bit', 0

times 510-($-$$) db 0
db 0x55, 0xAA