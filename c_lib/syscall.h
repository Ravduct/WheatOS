#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>
#include "print.h"
#include "bitmap.h"
#include "keyboard.h"
#include "multitask.h"
#include "time.h"

extern uint64_t user_code_base;
extern uint64_t user_stack_base;

static int user_addr_ok(uint64_t addr) {
    if (user_code_base && addr >= user_code_base && addr < user_code_base + page_size) {
        return 1;
    }
    if (user_stack_base && addr >= user_stack_base && addr < user_stack_base + page_size) {
        return 1;
    }
    return 0;
}

static int user_string_ok(uint64_t arg) {
    if (!user_addr_ok(arg)) return 0;
    for (uint64_t p = arg; user_addr_ok(p); p++) {
        if (*(const char *)p == '\0') return 1;
    }
    return 0;
}

uint64_t syscall_handler_c(uint64_t num, uint64_t arg) {
    switch (num) {
    case 1:
        if (user_string_ok(arg)) {
            print((const char *)arg);
        }
        return 0;
    case 2:
        task_exit();
        return 0;
    case 3:
        __asm__ volatile ("sti");
        sleep(arg);
        return 0;
    case 4:
        return (uint64_t)(uint8_t)kbd_read();
    default:
        return (uint64_t)-1;
    }
}

static inline void sys_print(const char *s) {
    __asm__ volatile ("int $0x80" : : "a"(1), "b"(s) : "memory");
}

static inline void sys_exit(void) {
    __asm__ volatile ("int $0x80" : : "a"(2) : "memory");
}

static inline void sys_sleep(uint64_t ms) {
    __asm__ volatile ("int $0x80" : : "a"(3), "b"(ms) : "memory");
}

static inline char sys_read(void) {
    uint64_t c;
    __asm__ volatile ("int $0x80" : "=a"(c) : "a"(4) : "memory");
    return (char)c;
}

#endif
