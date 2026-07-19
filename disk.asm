;main function
disk_load:
    pusha
    mov cx, 3

.retry:
    mov ah, 0x42
    mov dl, [boot_drive]
    mov si, DAP

    int 0x13
    jnc .success

    dec cx
    jnz .retry

    jmp .disk_error

.success:
    popa
    ret

.disk_error:
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