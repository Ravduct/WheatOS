#ifndef USER_H
#define USER_H

#include <stdint.h>
#include "heap.h"
#include "paging.h"
#include "user_syscall.h"

uint64_t user_code_base = 0;
uint64_t user_stack_base = 0;

extern void enter_user(uint64_t rip, uint64_t rsp);
extern char user_shell_start[];
extern char user_shell_end[];

void start_user_mode() {
    // Allocate memory for user code and stack
    uint8_t *code = (uint8_t *)alloc_page();
    uint8_t *stack = (uint8_t *)alloc_page();
    if (!code || !stack) return; // Return if allocation fails

    // Map the allocated pages into the virtual address space
    if (map_page((uint64_t)code, (uint64_t)code, 1) != 0) return;
    if (map_page((uint64_t)stack, (uint64_t)stack, 1) != 0) return;

    // Store the base addresses for user code and stack
    user_code_base = (uint64_t)code;
    user_stack_base = (uint64_t)stack;

    // Copy the user shell program into the allocated code memory
    uint64_t size = (uint64_t)(user_shell_end - user_shell_start);
    for (uint64_t i = 0; i < size; i++) {
        code[i] = (uint8_t)user_shell_start[i];
    }

    // Reload the CR3 register to ensure page table mappings are updated
    uint64_t cr3;
    __asm__ volatile ("mov %%cr3, %0" : "=r"(cr3)); // Read CR3
    __asm__ volatile ("mov %0, %%cr3" : : "r"(cr3) : "memory"); // Write CR3

    // Align the stack pointer to a 16-byte boundary
    uint64_t top = ((uint64_t)stack + page_size) & ~0xFULL;

    // Enter user mode with the user code entry point and stack pointer
    enter_user((uint64_t)code, top);
}

#endif
