[org 0x8000]
[bits 16]

xor ax, ax
mov ds, ax
mov es, ax
mov ss, ax
mov sp, 0x7C00
jmp stage2_main

%include "disk.asm"
%include "print.asm"
%include "e820.asm"
%include "a20.asm"
%include "gdt.asm"
%include "print_32bit.asm"

stage2_main:
    mov si, stage2_begin_msg
    call print_string
    call enable_a20
    call e820_memory_map

    mov word [sector_count], 1
    mov word [my_offset], 0x0010
    mov word [my_segment], 0xFFFF
    mov dword [lba_low], 4
    call disk_load

    cli
    lgdt [gdt_descriptor]

    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp CODE_SEG:protected_mode

[bits 32]
protected_mode:
    call enable_paging
    jmp long_mode

[bits 64]
long_mode:
    jmp halt

halt:
    ;mov si, stage2_exit_msg
    ;call print_string
    jmp $

stage2_begin_msg db "stage 2 ready", 0
stage2_exit_msg db "stage 2 exit,", 0
pm_msg db "Protected mode enabled", 0