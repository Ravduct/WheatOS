#ifndef MULTITASK_H
#define MULTITASK_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "heap.h"
#include "time.h"

extern void switch_task(uint64_t *old_rsp, uint64_t new_rsp);

typedef enum {
    task_running,
    task_ready,
    task_dead
} task_state;

typedef struct task{
    uint32_t id;
    uintptr_t rsp;
    task_state state;
    void* stack_bottom;
    struct task *next;
} task_t;

task_t *current_task = NULL;
task_t *task_list = NULL;
uint32_t next_task_id = 0;

#define stack_size 0x4000

void reap_dead_tasks() {
    if (!task_list) return;
    task_t *prev = NULL;
    task_t *current = task_list;

    while (current != NULL) {
        if (current->state == task_dead && current != current_task) {
            if (prev == NULL) {
                task_list = current->next;
            } else {
                prev->next = current->next;
            }
            if (current->stack_bottom) {
                kfree(current->stack_bottom);
            }
            kfree(current);
            current = (prev == NULL) ? task_list : prev->next;
        } else {
            prev = current;
            current = current->next;
        }
    }
}

void yield() {
    if (!current_task) return;
    
    reap_dead_tasks();

    task_t *next = current_task->next ? current_task->next : task_list;
    while (next != current_task && next->state != task_ready)
        next = next->next ? next->next : task_list;
    if (next == current_task) return;

    task_t *old = current_task;
    if (old->state == task_running) old->state = task_ready;
    next->state = task_running;
    current_task = next;
    switch_task((uint64_t *)&old->rsp, next->rsp);
}

void task_exit() { // Mark the current task as dead and yield to the next task
    __asm__ volatile ("cli");
    if (current_task) {
        current_task->state = task_dead;
    }
    __asm__ volatile ("sti");
    yield();
    while(1) __asm__ volatile ("hlt");
}

task_t *create_task(void (*entry_point)(void)) {
    task_t *new_task = (task_t *)kmalloc(sizeof(task_t));
    if (!new_task) return NULL;

    new_task->stack_bottom = kmalloc(stack_size);
    if (!new_task->stack_bottom) {
        kfree(new_task);
        return NULL;
    }

    new_task->id = next_task_id++;
    new_task->state = task_ready;
    new_task->next = NULL;

    uintptr_t top = ((uintptr_t)new_task->stack_bottom + stack_size) & ~0xFULL;
    uint64_t *stack = (uint64_t *)top;

    *(--stack) = (uint64_t)(uintptr_t)task_exit;
    *(--stack) = (uint64_t)(uintptr_t)entry_point;
    for (int i = 0; i < 6; i++) *(--stack) = 0ULL;

    new_task->rsp = (uint64_t)stack;

    if (task_list == NULL) {
        task_list = new_task;
    } else {
        task_t *temp = task_list;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = new_task;
    }

    return new_task;
}

#endif