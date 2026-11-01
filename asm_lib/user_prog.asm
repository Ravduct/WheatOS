[bits 64]

global user_shell_start
global user_shell_end

section .rodata

user_shell_start:
    incbin "build/user_shell.bin"
user_shell_end:
