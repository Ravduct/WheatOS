#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "../c_lib/user_syscall.h"
#include "../c_lib/fs.h"


void help(const char *args) {
    sys_print("Available commands:\n");
    sys_print("help - Show this help message\n");
    sys_print("exit - Exit the shell\n");
    sys_print("ls - List files in the current directory\n");
    sys_print("touch <filename> - Create a new file with the specified name\n");
}

void exit(const char *args) {
    sys_print("Exiting the shell...\n");
    sys_exit();
}

void ls(const char *args) {
    for (size_t i = 0; i < MAX_FILES; i++) {
        if (files[i].used) {
            sys_print(files[i].name);
            sys_print("\n");
        }
    }
}

void touch(const char *filename) {
    if (fs_create(filename) != NULL) {
        sys_print("File created: ");
        sys_print(filename);
        sys_print("\n");
    } else {
        sys_print("Failed to create file: ");
        sys_print(filename);
        sys_print("\n");
    }
}
#endif // FUNCTIONS_H