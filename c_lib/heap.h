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

struct block *head = NULL;

void *kmalloc(uint64_t byte) {
    if (byte == 0 || byte > page_size - sizeof(struct block)) {
        return NULL;
    }
    uint64_t prebyte = byte;
    byte = (byte + 7) & ~7; // Align to 8 bytes

    struct block *current = head;
    struct block *last = NULL;
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
        last = current;
        current = current->next;
    }
    void *new_page = alloc_page();
    if (new_page == NULL) return NULL;

    struct block *new_heap_block = (struct block *)new_page;
    new_heap_block->size = page_size - sizeof(struct block);
    new_heap_block->free = true;
    new_heap_block->next = NULL;

    if (last == NULL) {
        head = new_heap_block;
    } else {
        last->next = new_heap_block;
    }
    return kmalloc(prebyte);
}

void kfree(void *ptr) {
    if (ptr == NULL) return;
    struct block *b = (struct block *)((char *)ptr - sizeof(struct block));
    b->free = true;
    if (b->next != NULL && b->next->free && (char *)b + sizeof(struct block) + b->size == (char *)b->next) {
        b->size += sizeof(struct block) + b->next->size;
        b->next = b->next->next;
    }
}

#endif