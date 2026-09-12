ENTRY(entry_point_symbol)

SECTIONS {
    . = 0xD000

    .text : {
        *(.text)
    }
    .data : {
        *(.data)
    }
    .bss : {
        *(.bss)
    }
}