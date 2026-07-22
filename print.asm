; print.asm - simple BIOS teletype string printer
; Usage: call print_string

global print_string
print_string:
    ; Prints characters starting at label 'string' until a NUL byte
    push ds
    cld
    ; assume data is in same segment as code; load DS if needed by caller
    .print_loop:
        lodsb           ; AL = [DS:SI++]
        cmp al, 0
        je .done
        mov ah, 0x0E    ; BIOS teletype
        mov bh, 0x00
        mov bl, 0x07
        int 0x10
        jmp .print_loop
    .done:
        pop ds
        ret
