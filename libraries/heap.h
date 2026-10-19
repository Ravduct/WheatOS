#ifndef HEAP_H
#define HEAP_H

#include <stdint.h>
#include "bitmap.h"

struct block {
    uint64_t size;
    uint8_t is_free;
    struct block *next;
};

struct block *heap_start;

void heap_init(uint32_t total_pages) {
    heap_start = (struct block *)alloc_page(total_pages);
    heap_start->size = page_size - sizeof(struct block);
    heap_start->is_free = 1;
    heap_start->size = NULL;
}

void *kmalloc(uint64_t byte) {

}

#endif