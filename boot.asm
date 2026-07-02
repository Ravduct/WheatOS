[org 0x7C00]
[bits 16]

_start:
    jmp 0x0000: .normalized
.normalized:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    sti

    mov [boot_drive], dl

    mov si, switching_stage2_message
    call print_loop

    call check_extension
    call load_stage2

    jmp hang

load_stage2:
    mov cx, 3
.retry:
    mov ah, 0x42
    mov dl, [boot_drive]
    mov si, disk_address_packet
    int 0x13
    jnc .success
    dec cx
    jnz .retry
    jmp print_stage2_message
.success:
    jmp 0x0000:0x8000

print_loop:
    lodsb
    or al, al
    jz .done
    mov ah, 0x0E
    int 0x10
    jmp print_loop

.done: ret

disk_error:
    mov si, disk_error_message
    call print_loop
    jmp hang
hang:
    hlt
    jmp hang

switching_stage2_message: db 'Loading into Stage 2 bootloader', 0x0d, 0x0a, 0
disk_error_message: db 'Disk read error', 0x0d, 0x0a, 0
check_extension_message: db 'check_extension error', 0x0d, 0x0a, 0
load_stage2_message: db 'load_stage2 error', 0x0d, 0x0a, 0
print_extension_message:
    mov si, check_extension_message
    call print_loop
    jmp hang
print_stage2_message:
    mov si, load_stage2_message
    call print_loop
    jmp hang

boot_drive: db 0

check_extension:
    mov ah, 0x41
    mov bx, 0x55AA
    mov dl, [boot_drive]
    int 0x13
    jc disk_error
    cmp bx, 0xAA55
    jne disk_error
    ret

disk_address_packet:
    db 0x10
    db 0
    dw 1
    dw 0x8000
    dw 0x0000
    dq 1

times 510-($-$$) db 0
db 0x55, 0xAA