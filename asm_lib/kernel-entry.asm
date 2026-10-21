[bits 64]
;[org 0xD000]

global _start

extern _bss_start
extern _bss_end
extern kernel_main

_start:
    ;times 512 db 0x90

    mov al, dil
    add al, '0'

    mov rbx, 0xB8000
    mov byte [rbx], 'K'
    mov byte [rbx+1], 0x0F

    mov byte [rbx+2], al
    mov byte [rbx+3], 0x0F

    mov rax, _bss_start
    .loop:
        cmp rax, _bss_end
        je .done
        mov byte [rax], 0x0
        inc rax
        jmp .loop

    .done:
        call kernel_main
        jmp $