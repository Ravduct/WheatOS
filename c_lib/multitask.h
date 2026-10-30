#ifndef MULTITASK_H
#define MULTITASK_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "heap.h"
#include "time.h"

typedef enum {
    task_running,
    task_ready,
    task_dead
} task_state;

struct task {
    uint32_t id;
    uintptr_t rsp;
    task_state state;
    void* stack_bottom;
    struct task* next;
};

struct task *current_task = NULL;
struct task *task_list = NULL;
uint32_t next_task_id = 0;

#define stack_size 0x4000

struct task *create_task(void (*entry_point)(void)) {
    struct task *new_task = (struct task *)kmalloc(sizeof(struct task));
    new_task->id = next_task_id++;
    new_task->state = task_ready;
    new_task->stack_bottom = (void *)kmalloc(stack_size);
    new_task->rsp = (uintptr_t)new_task->stack_bottom + stack_size - sizeof(uintptr_t);
    *(uintptr_t*)new_task->rsp = (uintptr_t)entry_point; // Set the return address to the entry point
    new_task->next = NULL;

    // Add the new task to the task list
    if (task_list == NULL) {
        task_list = new_task;
    } else {
        struct task *temp = task_list;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = new_task;
    }

    return new_task;
}

#endif