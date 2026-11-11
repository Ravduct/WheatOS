#ifndef USER_SYSCALL_H
#define USER_SYSCALL_H

#include <stdint.h>
#include "fs_api.h"

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

/* Filesystem calls return -1 on invalid input, -2 on a disk save failure. */
static inline int sys_fs_call(uint64_t number, const void *arg) {
    uint64_t result;
    __asm__ volatile ("int $0x80" : "=a"(result) : "a"(number), "b"(arg) : "memory");
    return (int)result;
}

static inline int sys_fs_save(void) {
    return sys_fs_call(5, 0);
}

static inline int sys_fs_create(const char *name) {
    return sys_fs_call(6, name);
}

static inline int sys_fs_write(const char *name, const char *text) {
    fs_write_request request = {name, text};
    return sys_fs_call(7, &request);
}

static inline int sys_fs_remove(const char *name) {
    return sys_fs_call(8, name);
}

static inline int sys_fs_list(void) {
    return sys_fs_call(9, 0);
}

static inline int sys_fs_cat(const char *name) {
    return sys_fs_call(10, name);
}

#endif
