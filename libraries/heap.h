#ifndef HEAP_H
#define HEAP_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "bitmap.h"

struct block {
    uint64_t size;
    bool free;
    struct block *next;
};

struct block *head;

void kheap_init(void *heap_start, uint64_t heap_size) {
    head = (struct block *)heap_start;
    head->size = heap_size - sizeof(struct block);
    head->free = true;
    head->next = NULL;
}

void *kmalloc(uint64_t byte) {
    if (byte == 0) {
        return NULL;
    }
    uint64_t prebyte = byte;
    byte = (byte + 7) & ~7; // Align to 8 bytes

    struct block *current = head;
    while (current != NULL) {
        if (current->free && current->size >= byte) {
            if (current->size > byte + sizeof(struct block)) {
                struct block *new_block = (struct block *)((char *)current + sizeof(struct block) + byte);
                new_block->size = current->size - byte - sizeof(struct block);
                new_block->free = true;
                new_block->next = current->next;

                current->size = byte;
                current->next = new_block;
            }
            current->free = false;

            return (void *)((char *)current + sizeof(struct block));
        }
        current = current->next;
    }
    void *new_page = alloc_page(1);
    return kmalloc(prebyte);
}

#endif