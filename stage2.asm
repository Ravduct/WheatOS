[org 0x8000]
[bits 16]

%include "disk.asm"
%include "mbr.asm"

xor ax, ax
mov ds, ax
mov es, ax
mov ss, ax
mov sp, 0x7C00

check_a20:
    pushf
    push ds
    push es
    push di
    push si
    push ax
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
    je .enable_a20

    pop ax
    pop si
    pop di
    pop es
    pop ds
    popf
    ret

    .enable_a20:
        pop ax
        pop si
        pop di
        pop es
        pop ds
        popf

        ;code 
