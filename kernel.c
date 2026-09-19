void kernel_main(void *e820_map, int entry_count) {


    while (1) {}
}

void write_character(unsigned char c, unsigned char forecolor, unsigned char backcolor, int) {
    uint16_t attribute = (backcolor << 4) | (forecolor & 0x0F);

}