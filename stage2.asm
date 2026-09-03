[org 0x8000]
[bits 16]

xor ax, ax
mov ds, ax
mov es, ax
mov ss, ax
mov sp, 0x7C00
mov [boot_drive], dl

jmp stage2_main

%include "disk.asm"
%include "print.asm"
%include "e820.asm"
%include "a20.asm"
%include "gdt.asm"
%include "paging.asm"

[bits 16]
stage2_main:
    call enable_a20
    call e820_memory_map

    mov dl, [boot_drive]
    mov word [sector_count], 1
    mov word [my_offset], 0x0010
    mov word [my_segment], 0xFFFF
    mov dword [lba_low], 34
    call disk_load

    mov si, stage2_begin_msg
    call print_string
    jmp halt

    cli
    lgdt [gdt_descriptor]

    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp CODE_SEG:protected_mode

[bits 32]
protected_mode:
    ;mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov ss, ax

    ;call enable_paging
    ;mov ax, DATA_SEG64
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax
    ;jmp CODE_SEG64:long_mode

[bits 64]
long_mode:
    mov rax, 0x800000
    mov rsp, rax
    mov rbp, rax

    ;mov rsi, long_mode_msg
    ;call print_string_pm

    jmp halt

halt:
    ;mov si, stage2_exit_msg
    ;call print_string
    jmp $

stage2_loading db "loading stage 2...", 0
stage2_begin_msg db "stage 2 ready", 0
stage2_exit_msg db "stage 2 exit,", 0
protected_mode_msg db "Protected mode enabled", 0
long_mode_msg db "Long mode enabled", 0
boot_drive db 0