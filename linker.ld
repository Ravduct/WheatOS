ENTRY(_start)

SECTIONS {
    . = 0xD000;

    .text : {
        *(.text)
    }
    .data : {
        *(.data)
    }
    .bss : {
        _bss_start = .;
        *(.bss)
        _bss_end = .;
    }
}