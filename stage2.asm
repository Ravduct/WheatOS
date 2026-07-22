[org 0x8000]
[bits 16]

%include "disk.asm"
%include "print.asm"

xor ax, ax
mov ds, ax
mov es, ax
mov ss, ax
mov sp, 0x7C00

call enable_a20
mov si, "a20 enabled"
call print_string
jmp .halt

enable_a20:
    push ax
    push bx

    mov     ax, 0x2401
    int     0x15
    call check_a20
    cmp byte [a20_status], 1
    je .exit

    in al, 0x92
    or al, 2
    out 0x92, al
    call check_a20
    cmp byte [a20_status], 1
    je .exit

    cli
    
    mov al, 0xAD
    out 0x64, al
    .wait_kbc_input:
        in al, 0x64
        test al, 0x02
        jnz .wait_kbc_input
    mov al, 0xD0
    out 0x64, al
    .wait_again:
        in al, 0x64
        test al, 0x01
        jz .wait_again
    in al, 0x60
    mov bl, al
    mov al, 0xD1
    out 0x64, al
    .wait_again2:
        in al, 0x64
        test al, 0x02
        jnz .wait_again2
    mov al, bl
    or al, 0x02
    out 0x60, al
    .wait_again3:
        in al, 0x64
        test al, 0x02
        jnz .wait_again3
    mov al, 0xAE
    out 0x64, al

    sti
    call check_a20
    cmp byte [a20_status], 1
    je .exit

    jmp .halt

.exit:
    pop bx
    pop ax
    ret
.halt:
    jmp $

check_a20:
    pushf
    push ds
    push es
    push di
    push si
    push ax
    push bx
    push dx
    cli

    xor ax, ax
    mov ds, ax
    mov si, 0x0500

    not ax
    mov es, ax
    mov di, 0x0510

    mov dl, byte [ds:si] ; = dl

    mov bl, byte [es:di] ; claude tell me to
    not dl
    mov byte [es:di], dl ; = -dl
    mov al, byte [ds:si]

    mov byte [es:di], bl ; claude tell me to
    
    cmp al, dl
    jne .a20_is_on

    mov byte [a20_status], 0
    sti
    pop dx
    pop bx
    pop ax
    pop si
    pop di
    pop es
    pop ds
    popf
    ret

    .a20_is_on:
        mov byte [a20_status], 1
        sti
        pop dx
        pop bx
        pop ax
        pop si
        pop di
        pop es
        pop ds
        popf
        ret

a20_status: db 0