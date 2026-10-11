#ifndef TIME_H
#define TIME_H

#include <stdint.h>
#include "pic.h"

volatile uint64_t ticks = 0;

void timer_handler() {
    ticks++;
}

void set_pit() {
    uint32_t divisor = 1193182 / 100;
    uint8_t low_byte = divisor & 0xFF;
    uint8_t high_byte = (divisor >> 8) & 0xFF;
    outb(0x43, 0x36);
    outb(0x40, low_byte);
    outb(0x40, high_byte);
}

void sleeep(uint64_t time) {
    
}

#endif TIME_H