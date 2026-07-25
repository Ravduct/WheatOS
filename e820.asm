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

entry_count: db 0
e820_error_msg db "error at e820", 0
e820_finish_msg db "e820 finished", 0
e820_buffer: times 768 db 0