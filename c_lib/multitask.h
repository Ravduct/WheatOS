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

#endif