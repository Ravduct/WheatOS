#ifndef TIME_H
#define TIME_H

#include <stdint.h>
#include "pic.h"
#include "multitask.h"
#include "keyboard.h"

#define PIT_BASE_FREQUENCY 1193182
#define PIT_COMMAND_PORT 0x43
#define PIT_CHANNEL0_PORT 0x40
volatile uint64_t ticks = 0;

void timer_handler() {
    ticks++;
}

void set_pit() {
    uint32_t divisor = 1193182 / 100;
    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);
}

void sleep(uint64_t ms) {
    uint64_t start = ticks;
    uint64_t wait_ticks = (ms/10 + 9) / 10;

    while (ticks - start < wait_ticks) {
        __asm__ volatile ("hlt");
    }
}

void irq_handler_c(uint64_t vector) {
    switch (vector)
    {
    case 33:
        keyboard_handler(vector);
        break;
    case 32:
        timer_handler();
        outb(0x20, 0x20);
        yield();
        return;
    default:
        break;
    }

    if (vector >= 40) {
        outb(0xA0, 0x20);
    }

    outb(0x20, 0x20);
}

#endif