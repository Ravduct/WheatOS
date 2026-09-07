[bits 64]
[org 0xD000]

times 512 db 0x90

mov rbx, 0xB8000
mov byte [rbx], 'K'
mov byte [rbx+1], 0x0F
jmp $