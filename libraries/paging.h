#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>
#include "print.h"
#include "bitmap.h"

#define page_present 0x1
#define page_writable 0x2
#define page_huge 0x80

uint64_t pml4_index(uint64_t virt) {
    return (virt >> 39) & 0x1FF;
}

uint64_t pdpt_index(uint64_t virt) {
    return (virt >> 30) & 0x1FF;
}

uint64_t pd_index(uint64_t virt) {
    return (virt >> 21) & 0x1FF;
}

static inline uint64_t read_cr3() {
    uint64_t value;
    __asm__ volatile ("mov %%cr3, %0" : "=r"(value));
    return value;
}

static inline uint64_t *get_pml4() {
    return (uint64_t *)(read_cr3() & ~0xFFFULL);
}

uint64_t *next_table(uint64_t *table, uint64_t index, uint32_t total_pages) {
    if (!(table[index] & page_present)) {
        uint64_t *new_table = (uint64_t *)alloc_page(total_pages);
        if (new_table == NULL) return NULL;
        for (int i = 0; i < 512; i++) {
            new_table[i] = 0;
        }
        table[index] = (uint64_t)new_table | page_present | page_writable;
    }
    return (uint64_t *)(table[index] & ~0xFFFULL);
}

static inline void invlpg(uint64_t addr) {
    __asm__ volatile ("invlpg (%0)" : : "r"(addr) : "memory");
}

int map_page(uint64_t virt, uint64_t phys, uint32_t total_pages) {
    if (virt & 0x1FFFFF || phys & 0x1FFFFF) return -1;

    uint64_t *pml4 = get_pml4();
    uint64_t *pdpt = next_table(pml4, pml4_index(virt), total_pages);
    if (pdpt == NULL) return -1;

    uint64_t *pd = next_table(pdpt, pdpt_index(virt), total_pages);
    if (pd == NULL) return -1;

    pd[pd_index(virt)] = phys | page_present | page_writable | page_huge;
    invlpg(virt);
    return 0;
}

#endif