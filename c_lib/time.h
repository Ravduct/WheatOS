#ifndef TIME_H
#define TIME_H

#include <stdint.h>
#include "pic.h"
#include "multitask.h"

#define PIT_BASE_FREQUENCY 1193182
#define PIT_COMMAND_PORT 0x43
#define PIT_CHANNEL0_PORT 0x40
volatile uint64_t ticks = 0;

void timer_handler() {
    ticks++;
}

void sleep(uint64_t ms) {
    uint64_t start = ticks;
    uint64_t wait_ticks = (ms/10 + 9) / 10;

    while (ticks - start < wait_ticks) {
        __asm__ volatile ("hlt");
    }
}

#endif