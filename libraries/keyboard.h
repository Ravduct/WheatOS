#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include "print.h"

static bool shift_pressed = false;
static const char keymap_lowercase[] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' '
};
static const char keymap_uppercase[] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0,  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0,
    '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' '
};
char keyboard_to_ascii(uint8_t scancode) {
    switch (scancode) {
        case 0x2A:
        case 0x36:
            shift_pressed = true;
            return 0;
        
        case 0xAA:
        case 0xB6:
            shift_pressed = false;
            return 0;
    }

    if (scancode & 0x80) {
        return 0;
    }

    if (scancode >= sizeof(keymap_lowercase)) {
        return 0;
    }

    char ascii = shift_pressed ? keymap_uppercase[scancode] : keymap_lowercase[scancode];
    return ascii;
}

void keyboard_handler(uint64_t vector) {
    uint8_t scancode = inb(0x60);

    char c = keyboard_to_ascii(scancode);
    if (c != 0) {
        char str[2] = {c, '\0'};
        print(str);
    }
    outb(0x20, 0x20); // Send End of Interrupt (EOI) signal to PIC
}
#endif