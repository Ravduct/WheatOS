#include <stdint.h>

char *int_to_str(uint64_t number);
void update_hardware_cursor(uint8_t x, uint8_t y);
void write_character(unsigned char c, unsigned char forecolor, unsigned char backcolor);
void print(const char *string);
void clear_screen();
void new_line();