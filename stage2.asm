[org 0x8000]
[bits 16]

_start2:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    sti
    mov [boot_drive], dl
    mov si, stage2_message
    call print_loop
    jmp stage2_main

stage2_main:
    enable_a20
    jmp $

print_loop:
    lodsb
    cmp al, 0
    jz .done
    mov ah, 0x0E
    int 0x10
    jmp print_loop
.done: ret
.done_a20:
    mov si, msg_a20_ok
    call print_loop
    ret

enable_a20:
    call check_a20
    cmp ax, 1
    je .done_a20

    mov ax, 0x2401
    int 0x15
    call check_a20
    cmp ax, 1
    je .done_a20

    in al, 0x92
    or al, 2
    and al, 0xFE
    out 0x92, al
    call check_a20
    cmp ax, 1
    je .done_a20

    call a20_wait_input
    mov al, 0xAD
    out 0x64, al

    call a20_wait_input
    mov al, 0xD0
    out 0x64, al

    call a20_wait_output
    mov al, 0xD0
    out 0x60, al

    call a20_wait_input
    mov al, 0xD1
    out 0x64, al

    call a20_wait_input
    pop ax
    or al, 2
    out 0x60, al

    call a20_wait_input
    mov al, 0xAE
    out 0x64, al

    call a20_wait_input
    call check_a20
    cmp ax, 1
    je .done_a20

    mov si, msg_a20_fail
    call print_loop
    jmp $

a20_wait_input:
    in al, 0x64
    test al, 2
    jnz a20_wait_input
    ret

a20_wait_output:
    in al, 0x64
    test al, 1
    jnz a20_wait_output
    ret

check_a20:
    pushf
    push ds
    push es
    push di
    push si

    cli

    xor ax, ax
    mov es, ax
    mov di, 0x500

    mov ax, 0xFFFF
    mov ds, ax
    mov si, 0x0510

    mov al, byte [es:di]
    push ax
    
    mov al, byte [ds:si]
    push ax

    mov byte [es:di], 0x00
    mov byte [ds:si], 0xFF

    cmp byte [es:di], 0xFF

    pop ax
    mov byte [es:di], al

    pop ax
    mov byte [ds:si], al

    mov ax, 0
    je .exit

    mov ax, 1
.exit:
    pop si
    pop di
    pop es
    pop ds
    popf
    ret

stage2_message: db 'Stage 2 bootloader loaded', 0x0d, 0x0a, 0
msg_a20_ok: db 'A20 line enabled', 0x0d, 0x0a, 0
msg_a20_fail: db 'A20 line failed to enable', 0x0d, 0x0a, 0
boot_drive: db 0
