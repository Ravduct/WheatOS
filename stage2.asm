[org 0x8000]
[bits 16]

_start:
    mov si, stage2_message
    call print_loop
    hang:
        hlt
        jmp hang

print_loop:
    lodsb
    cmp al, 0
    jz .done
    mov ah, 0x0E
    int 0x10
    jmp print_loop
.done: ret

stage2_message: db 'Stage 2 bootloader loaded', 0x0d, 0x0a, 0