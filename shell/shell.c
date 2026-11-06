#include "../c_lib/user_syscall.h"
#include <stddef.h>
#include <stdint.h>
#include "functions.h"

typedef void (*command_func)(const char *args);

typedef struct {
    const char *name;
    command_func func;
} command_entry;

static int string_equals(const char *str1, const char *str2) {
    while (*str1 && *str2) {
        if (*str1 != *str2) {
            return 0;
        }
        str1++;
        str2++;
    }
    return *str1 == *str2;
}

static command_func find_command(const command_entry *command_list, size_t list_size, const char *input) {
    for (size_t i = 0; i < list_size; i++) {
        if (string_equals(input, command_list[i].name)) {
            return command_list[i].func;
        }
    }
    return NULL;

}

void shell() {
    const command_entry command_list[] = {
        {"help", help},
        {"exit", exit},
        {"ls", ls},
        {"touch", touch},
        {"cat", cat},
        {"write", write},
        {"rm", rm},
        {"echo", echo}
    };

    sys_print("Welcome to WheatOS!\n");
    while (1) {
        sys_print("> ");
        char command[80];
        unsigned int length = 0;

        while (1) {
            char c = sys_read();
            if (c == '\n' || c == '\r') {
                sys_print("\n");
                break;
            }
            if (c == '\b' || c == 127) {
                if (length > 0) {
                    length--;
                    sys_print("\b \b");
                }
                continue;
            }
            if (length < sizeof(command) - 1) {
                command[length++] = c;
                char echo[2] = {c, '\0'};
                sys_print(echo);
            }
        }

        command[length] = '\0';

        char *args = command;
        while (*args && *args != ' ') {
            args++;
        }

        if (*args == ' ') {
            *args++ = '\0';             // End the command name here.
            while (*args == ' ') args++; // Skip extra spaces before the argument.
        }

        command_func cmd = find_command(
            command_list,
            sizeof(command_list) / sizeof(command_list[0]),
            command
        );

        if (cmd) {
            cmd(*args ? args : NULL);
        } else if (length == 0) {
            //sys_print("Unknown command. Type 'help' for a list of commands.\n");
        } else {
            sys_print("Unknown command. Type 'help' for a list of commands.\n");
        }
    }
}
