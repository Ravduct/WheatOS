[bits 64]
global load_gdt

global load_idt
global default_exception_handler

load_gdt:
    ; Load the GDT
    lgdt [rdi]

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax

    push 0x08
    lea rax, [.reload_cs]
    push rax
    retfq
.reload_cs:
    ret

[bits 64]
load_idt:
    ; Load the IDT
    lidt [rdi]
    ret

default_exception_handler:
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    
    add rsp, 16

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    iretq