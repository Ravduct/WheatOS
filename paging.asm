align 4096
pml4_table:
    dq pdpt_table + 0x3
    times 511 dq 0

align 4096
pdpt_table:
    dq pd_table + 0x3
    times 511 dq 0

align 4096
pd_table:
    %assign i 0
    %rep 512
        dq (i * 0x200000) + 0x83
        %assign i i + 1
    %endrep
enable_paging:
    mov eax, pml4_table
    mov cr3, eax

    mov eax, cr4
    bts eax, 5
    mov cr4, eax

    