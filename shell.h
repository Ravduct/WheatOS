#ifndef SHELL_H
#define SHELL_H

#include "c_lib/syscall.h"

void shell() {
    sys_print("Welcome to the shell!\n");
    while (1) {
        sys_print("> ");
        char c = sys_read();
        if (c == 'q') {
            sys_exit();
        }
        sys_print("You typed: ");
        sys_print((char[]){c, '\0'});
        sys_print("\n");
    }
}

#endif