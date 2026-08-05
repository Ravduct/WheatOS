[org 0x8000]
[bits 16]

xor ax, ax
mov ds, ax
mov es, ax
mov ss, ax
mov sp, 0x7C00

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

    jmp halt

%include "disk.asm"
%include "print.asm"
%include "e820.asm"
%include "a20.asm"

halt:
    mov si, stage2_exit_msg
    call print_string
    jmp $

stage2_begin_msg db "stage 2 ready", 0
stage2_exit_msg db "stage 2 exit", 0