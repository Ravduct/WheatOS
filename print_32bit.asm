print_string_pm:
    ; Prints a null-terminated string directly to video memory
    ; Usage: mov esi, <address of string>
    ;        call print_string_pm
    push eax
    push ebx

    mov ebx, 0xB8000        ; video memory start (top-left of screen)
    .loop:
        mov al, [esi]         ; load next character
        cmp al, 0
        je .done
        mov [ebx], al          ; write character byte
        mov byte [ebx+1], 0x0F   ; write attribute byte (white on black)
        add ebx, 2                  ; advance to next screen cell
        inc esi                       ; advance to next character in string
        jmp .loop
    .done:
        pop ebx
        pop eax
        ret