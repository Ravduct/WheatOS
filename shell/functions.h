#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "../c_lib/user_syscall.h"
#include <stddef.h>

void help(const char *args) {
    sys_print("Available commands:\n");
    sys_print("help - Show this help message\n");
    sys_print("exit - Exit the shell\n");
    sys_print("ls - List files in the current directory\n");
    sys_print("touch <filename> - Create a new file with the specified name\n");
    sys_print("cat <filename> - Show a file's contents\n");
    sys_print("write <filename> <text> - Create or replace a file's contents\n");
    sys_print("rm <filename> - Remove a file\n");
    sys_print("echo <text> - Print text\n");
}

void exit(const char *args) {
    sys_print("Exiting the shell...\n");
    sys_exit();
}

void ls(const char *args) {
    sys_fs_list();
}

void touch(const char *filename) {
    if (filename == NULL) {
        sys_print("Usage: touch <filename>\n");
        return;
    }

    int result = sys_fs_create(filename);
    if (result == -2) {
        sys_print("File changed in memory, but saving to disk failed\n");
    } else if (result == 0) {
        sys_print("File created: ");
        sys_print(filename);
        sys_print("\n");
    } else {
        sys_print("Failed to create file: ");
        sys_print(filename);
        sys_print("\n");
    }
}

void cat(const char *filename) {
    if (filename == NULL) {
        sys_print("Usage: cat <filename>\n");
        return;
    }

    if (sys_fs_cat(filename) != 0) {
        sys_print("File not found: ");
        sys_print(filename);
        sys_print("\n");
        return;
    }

}

void write(const char *args) {
    if (args == NULL) {
        sys_print("Usage: write <filename> <text>\n");
        return;
    }

    const char *text = args;
    while (*text && *text != ' ') text++;
    if (*text == '\0') {
        sys_print("Usage: write <filename> <text>\n");
        return;
    }

    size_t filename_length = (size_t)(text - args);
    if (filename_length >= NAME_SIZE) {
        sys_print("Filename is too long\n");
        return;
    }

    char filename[NAME_SIZE];
    for (size_t i = 0; i < filename_length; i++) {
        filename[i] = args[i];
    }
    filename[filename_length] = '\0';

    while (*text == ' ') text++;
    if (*text == '\0') {
        sys_print("Usage: write <filename> <text>\n");
        return;
    }

    size_t length = 0;
    while (text[length] != '\0') length++;
    if (length >= DATA_SIZE) {
        sys_print("Text is too long\n");
        return;
    }

    int result = sys_fs_write(filename, text);
    if (result == -2) {
        sys_print("File changed in memory, but saving to disk failed\n");
        return;
    }
    if (result != 0) {
        sys_print("Failed to write file: ");
        sys_print(filename);
        sys_print("\n");
        return;
    }

    sys_print("Wrote file: ");
    sys_print(filename);
    sys_print("\n");
}

void rm(const char *filename) {
    if (filename == NULL) {
        sys_print("Usage: rm <filename>\n");
        return;
    }

    int result = sys_fs_remove(filename);
    if (result == -2) {
        sys_print("File changed in memory, but saving to disk failed\n");
    } else if (result == 0) {
        sys_print("Removed file: ");
        sys_print(filename);
        sys_print("\n");
    } else {
        sys_print("File not found: ");
        sys_print(filename);
        sys_print("\n");
    }
}

void echo(const char *text) {
    if (text != NULL) sys_print(text);
    sys_print("\n");
}
#endif
