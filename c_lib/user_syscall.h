#ifndef USER_SYSCALL_H
#define USER_SYSCALL_H

#include <stdint.h>

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

static inline void sys_fs_save(void) {
    __asm__ volatile ("int $0x80" : : "a"(5) : "memory");
}

#endif
