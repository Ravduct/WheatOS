[bits 64]
default rel

global user_shell_start
global user_shell_end

section .rodata

user_shell_start:
    mov rax, 1
    lea rbx, [banner]
    int 0x80

.repl:
    mov rax, 1
    lea rbx, [prompt]
    int 0x80

    xor r12, r12
    lea r13, [line]

.read:
    mov rax, 4
    int 0x80

    cmp al, 8
    je .back
    cmp al, 127
    je .back
    cmp al, 10
    je .submit
    cmp al, 13
    je .submit

    cmp r12, 79
    jae .read

    mov [r13 + r12], al
    inc r12

    push rax
    mov rax, 1
    mov rbx, rsp
    int 0x80
    pop rax
    jmp .read

.back:
    test r12, r12
    jz .read
    dec r12
    mov rax, 1
    lea rbx, [bs]
    int 0x80
    jmp .read

.submit:
    mov byte [r13 + r12], 0
    mov rax, 1
    lea rbx, [nl]
    int 0x80

    test r12, r12
    jz .repl

    lea rsi, [line]
    lea rdi, [cmd_exit]
    call .streq
    test rax, rax
    jnz .do_exit

    lea rsi, [line]
    lea rdi, [cmd_help]
    call .streq
    test rax, rax
    jnz .do_help

    mov rax, 1
    lea rbx, [unknown]
    int 0x80
    jmp .repl

.do_help:
    mov rax, 1
    lea rbx, [help]
    int 0x80
    jmp .repl

.do_exit:
    mov rax, 2
    int 0x80
    jmp $

.streq:
    push rsi
    push rdi
.streq_loop:
    mov al, [rsi]
    mov bl, [rdi]
    cmp al, bl
    jne .streq_no
    test al, al
    jz .streq_yes
    inc rsi
    inc rdi
    jmp .streq_loop
.streq_yes:
    mov rax, 1
    pop rdi
    pop rsi
    ret
.streq_no:
    xor rax, rax
    pop rdi
    pop rsi
    ret

banner  db "WheatOS shell", 10, "commands: help, exit", 10, 0
prompt  db "> ", 0
nl      db 10, 0
bs      db 8, 0
unknown db "unknown command", 10, 0
help    db "help  list commands", 10, "exit  leave the shell", 10, 0
cmd_exit db "exit", 0
cmd_help db "help", 0
line    times 80 db 0

user_shell_end:
