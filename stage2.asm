[org 0x8000]
[bits 16]

xor ax, ax
mov ds, ax
mov es, ax
mov ss, ax
mov sp, 0x7C00

stage2_main:
    mov si, stage2_begin_msg
    call print_string
    call enable_a20
    mov si, a20_ready
    call print_string
    call e820_memory_map

    jmp halt

%include "disk.asm"
%include "print.asm"

e820_memory_map:
    push eax
    push edx
    push ecx
    push es
    push di
    push ebx

    xor ax, ax
    xor ebx, ebx
    mov es, ax
    mov di, e820_buffer
    e820_loop:
        mov eax, 0xE820
        mov edx, 0x534D4150
        mov ecx, 24
        int 0x15
        jc .e820_error

        cmp eax, 0x534D4150
        jne .e820_error

        inc byte [entry_count]
        add di, 24

        cmp ebx, 0
        jne e820_loop
        je .e820_exit
    .e820_exit:
        pop ebx
        pop di
        pop es
        pop ecx
        pop edx
        pop eax
        mov si, e820_finish_msg
        call print_string
        ret

    .e820_error:
        mov si, e820_error_msg
        call print_string
        jmp $

enable_a20:
    push ax
    push bx

    mov ax, 0x2401
    int 0x15
    call check_a20
    cmp byte [a20_status], 1
    je .a20_exit

    in al, 0x92
    or al, 2
    out 0x92, al
    call check_a20
    cmp byte [a20_status], 1
    je .a20_exit

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
    je .a20_exit

    jmp halt

.a20_exit:
    pop bx
    pop ax
    ret
halt:
    mov si, stage2_exit_msg
    call print_string
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
entry_count: db 0
stage2_begin_msg db "stage 2 ready", 0
a20_ready db "a20 enabled", 0
stage2_exit_msg db "stage 2 exit", 0
e820_error_msg db "error at e820", 0
e820_finish_msg db "e820 finished", 0
e820_buffer: times 768 db 0