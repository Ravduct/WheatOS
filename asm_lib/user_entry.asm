[bits 64]

section .text.entry

global user_entry
extern shell

user_entry:
    call shell
    mov eax, 2
    int 0x80
.halt:
    jmp .halt
