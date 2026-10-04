#include <stdint.h>
#include <print.h>

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
void update_hardware_cursor(uint8_t x, uint8_t y) {
    uint16_t position = (y * 80) + x;

    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)(position >> 8) & 0xFF);

    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(position & 0xFF));
}
void write_character(unsigned char c, unsigned char forecolor, unsigned char backcolor) {
    uint16_t attrib = (backcolor << 4) | (forecolor & 0x0F);
    volatile uint16_t * where;
    if (c == '\n' || x >= 80) {
        x = 0;
        y += 1;
        update_hardware_cursor(x, y);
        if (c == '\n') {
            return;
        }
    }
    if(c == '\b') {
        if (x == 0 && y > 0) {
            y -= 1;
            x = 79;
            volatile uint16_t *vga_buffer = (volatile uint16_t *)0xB8000;
            while (x > 0) {
                uint16_t cell = vga_buffer[y * 80 + x];
                unsigned char ch = (unsigned char)(cell & 0xFF);

                if (ch != ' ' && ch != '\0') {
                    if (x < 79) {
                        x += 1;
                    }
                    break;
                }
                x--;
            }
        } else {
            x -= 1;
        }
        where = (volatile uint16_t *)0xB8000 + (y * 80 + x);
        *where = ' ' | (attrib << 8);
        update_hardware_cursor(x, y);
        return;
    }
    where = (volatile uint16_t *)0xB8000 + (y * 80 + x);
    *where = c | (attrib << 8);
    x += 1;
    update_hardware_cursor(x, y);
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