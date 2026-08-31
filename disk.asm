;main function
[bits 16]

disk_load:
    mov si, disk_start_msg
    call print_string
    pusha
    mov cx, 3

    .retry:
        mov si, disk_looping_msg
        call print_string
        ; dl = boot_drive
        mov ah, 0x42
        mov si, DAP

        int 0x13
        mov si, disk_stable_msg
        call print_string
        jnc .success

        dec cx
        jnz .retry

        jmp .disk_error

    .success:
        popa
        mov si, disk_succeed_msg
        call print_string
        ret

    .disk_error:
        mov si, disk_error_msg
        call print_string
        jmp $

    ;variables
    DAP:
        packet_size db 0x10
        reserved db 0x00
        sector_count dw 0x0
        my_offset dw 0x0000
        my_segment dw 0x0000
        lba_low dd 0x00000000
        lba_high dd 0x00000000

disk_start_msg db "disk load start", 0
disk_error_msg db "disk load error", 0
disk_looping_msg db "disk load looping", 0
disk_succeed_msg db "disk load succeed", 0
disk_stable_msg db "disk load stable", 0