; print.asm - simple BIOS teletype string printer
; Usage: call print_string

global print_string
print_string:
    ; Prints characters starting at label 'string' until a NUL byte
    push si
    push ax
    push bx
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
        mov al, 0x0D    ; carriage return
        mov ah, 0x0E
        mov bh, 0x00
        mov bl, 0x07
        int 0x10

        mov al, 0x0A    ; line feed
        mov ah, 0x0E
        mov bh, 0x00
        mov bl, 0x07
        int 0x10

        pop bx
        pop ax
        pop si
        ret
