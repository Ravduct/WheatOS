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
    cli
    hlt
    jmp $