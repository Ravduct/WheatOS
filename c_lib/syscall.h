#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>
#include "print.h"
#include "bitmap.h"
#include "keyboard.h"
#include "multitask.h"
#include "time.h"
#include "user_syscall.h"
#include "fs.h"

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
    case 5:
        return fs_save() == 0 ? 0 : (uint64_t)-2;
    case 6:
        if (!user_string_ok(arg) || fs_create((const char *)arg) == NULL) {
            return (uint64_t)-1;
        }
        return fs_save() == 0 ? 0 : (uint64_t)-2;
    case 7: {
        for (size_t i = 0; i < sizeof(fs_write_request); i++) {
            if (arg + i < arg || !user_addr_ok(arg + i)) return (uint64_t)-1;
        }
        fs_write_request request = *(const fs_write_request *)arg;
        if (!user_string_ok((uint64_t)request.name) ||
            !user_string_ok((uint64_t)request.text)) return (uint64_t)-1;
        size_t length = 0;
        while (request.text[length]) {
            if (++length >= DATA_SIZE) return (uint64_t)-1;
        }
        File *file = fs_find(request.name);
        if (file == NULL) file = fs_create(request.name);
        if (file == NULL || fs_write(file, request.text, length) != 0) {
            return (uint64_t)-1;
        }
        return fs_save() == 0 ? 0 : (uint64_t)-2;
    }
    case 8:
        if (!user_string_ok(arg) || fs_remove((const char *)arg) != 0) {
            return (uint64_t)-1;
        }
        return fs_save() == 0 ? 0 : (uint64_t)-2;
    case 9:
        for (size_t i = 0; i < MAX_FILES; i++) {
            if (files[i].used) {
                print(files[i].name);
                print("\n");
            }
        }
        return 0;
    case 10: {
        if (!user_string_ok(arg)) return (uint64_t)-1;
        File *file = fs_find((const char *)arg);
        if (file == NULL) return (uint64_t)-1;
        print(file->data);
        print("\n");
        return 0;
    }
    default:
        return (uint64_t)-1;
    }
}

#endif
