#ifndef FUNCTIONS_H
#define FUNCTIONS_H

void help() {
    sys_print("Available commands:\n");
    sys_print("help - Show this help message\n");
    sys_print("exit - Exit the shell\n");
}

void exit() {
    sys_print("Exiting the shell...\n");
    sys_exit();
}

#endif // FUNCTIONS_H