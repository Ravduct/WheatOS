setup_paging:

align 4096
pml4_table:
    dq pdpt_table | 11b ; Present, Read/Write
    times 511 dq 0

align 4096
pdpt_table:
    dq pd_table | 11b ; Present, Read/Write
    times 511 dq 0

align 4096
pd_table:
    %assign addr 0
    %rep 512
        dq addr | 10000011b ; Present, Read/Write, 2MB page
        %assign addr addr + 0x200000
    %endrep
