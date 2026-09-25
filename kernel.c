#include <inttypes.h>
#define page_size 0x200000;

uint8_t x = 0;
uint8_t y = 0;
char buffer[21];

char *int_to_str(uint64_t number) {
    if (number == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
    }
    //find length
    uint64_t floor_num = 10;
    int i = 1;
    while(number / floor_num != 0) {
        i += 1;
        floor_num *= 10;
    }
    int length = i+1;

    floor_num = 1;
    uint64_t mod_num = 10;
    buffer[length - 1] = '\0';
    //find hex value
    for (int i = length-2; i >= 0; i--) {
        buffer[i] = (char)(number % mod_num / floor_num) + 48;
        floor_num *= 10;
        mod_num *= 10;
    }
    return buffer;
}

void write_character(unsigned char c, unsigned char forecolor, unsigned char backcolor) {
    if(c == '\n' || x >= 80) {
        x = 0;
        y += 1;
        if (c == '\n') {
            return;
        }
    }
    uint16_t attrib = (backcolor << 4) | (forecolor & 0x0F);
    volatile uint16_t * where;
    where = (volatile uint16_t *)0xB8000 + (y * 80 + x);
    *where = c | (attrib << 8);
    x += 1;
}

void print(const char *string) {
    while (*string) {
        write_character(*string, 7, 0);
        string++;
    }
}

void clear_screen() {
    x = y = 0;
    for(int i = 0; i < (80*25); i++){
        write_character(' ', 7, 0);
    }
    x = y = 0;
}
void new_line() {
    write_character('\n', 7, 0);
}

struct e820_entry {
    uint64_t base_address;
    uint64_t length;
    uint32_t type;
    uint32_t extended_attribute;
} __attribute__((packed));

void kernel_main(void *e820_map, int entry_count) {
    clear_screen();
    print("hello world\n\n");
    
    struct e820_entry *entries = (struct e820_entry *)e820_map;

    uint64_t highest_address = 0;
    for (int i = 0; i < entry_count; i++) {
        uint64_t end_address = entries[i].base_address + entries[i].length;
        if (end_address > highest_address) {
            highest_address = end_address;
        }
    }
    uint32_t total_pages = highest_address / page_size;
    

    while (1) {}
}