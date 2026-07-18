disk_load:
    pusha
    push dx

    mov ah, 0x02
    mov al, dh
    mov cl, 0x02

    mov ch, 0x00
    mov dh, 0x00

    int 0x13
    jc .disk_error
    
    pop dx
    cmp al, dh

    jne .sector_error
    popa
    ret

.disk_error:
    jmp .disk_loop
.sector_error:
    jmp .disk_loop

.disk_loop:
    jmp $
DAP:
    packet_size db 0x10
    reserved db 0x00
    sector_count dw 0x00
    off-set dw 0x0000
    segment dw 0x0000