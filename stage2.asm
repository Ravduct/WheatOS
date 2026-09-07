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

    push ax
    push bx
    push ds
    push es
    push si
    push di

    xor ax, ax
    mov ds, ax
    mov si, 0x0500

    mov ax, 0xffff
    mov es, ax
    mov di, 0x0510

    mov byte [ds:si], 0x42
    mov al, [es:di]

    cmp al, 0x42
    je .a20_check_fail

    mov si, a20_check_pass_msg
    call print_string
    jmp .a20_check_done

    .a20_check_fail:
    mov si, a20_check_fail_msg
    call print_string

    .a20_check_done:
    pop di
    pop si
    pop es
    pop ds
    pop bx
    pop ax

    mov dl, [boot_drive]
    mov word [sector_count], 1
    mov word [my_offset], 0xD000
    mov word [my_segment], 0x0
    mov dword [lba_low], 34
    call disk_load

    cli
    lgdt [gdt_descriptor]

    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp CODE_SEG:protected_mode

[bits 32]
protected_mode:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov ss, ax

    call enable_paging
    mov ax, DATA_SEG64
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax
    jmp CODE_SEG64:long_mode

[bits 64]
long_mode:
    mov rax, 0x800000
    mov rsp, rax
    mov rbp, rax

    mov rsi, long_mode_msg
    call print_string_lm

    jmp 0xD000

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

a20_check_pass_msg db "A20 line enabled", 0
a20_check_fail_msg db "A20 line not enabled", 0