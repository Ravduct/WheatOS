#include <stdint.h>
#include <stddef.h>
#define page_size 0x200000ULL
uint8_t bitmap[1024];

void set_bit(uint8_t *bitmap, uint32_t page_number) {
    uint32_t byte_index = page_number / 8;
    uint16_t bit_position = page_number % 8;
    uint8_t mask = 1 << bit_position;

    bitmap[byte_index] = bitmap[byte_index] | mask;
}

void clear_bit(uint8_t *bitmap, uint32_t page_number) {
    uint32_t byte_index = page_number / 8;
    uint16_t bit_position = page_number % 8;
    uint16_t mask = 1 << bit_position;

    bitmap[byte_index] &= ~mask;
}

uint8_t test_bit(uint8_t *bitmap, uint32_t page_number) {
    uint32_t byte_index = page_number / 8;
    uint16_t bit_position = page_number % 8;
    uint16_t mask = 1 << bit_position;

    return bitmap[byte_index] & mask;
}

void *alloc_page(uint32_t total_page) {
    for (int i = 0; i < total_page; i++) {
        if (test_bit(bitmap, i) == 0) {
            set_bit(bitmap, i);
            uint64_t address = (uint64_t)i * page_size;
            return (void *)(uintptr_t)address;
        }
    }
    return NULL;
}

void free_page(void *ptr) {
    uint64_t address = (uint64_t)(uintptr_t)ptr;

    if (address % page_size == 0 && test_bit(bitmap, address/page_size) != 0) {
        clear_bit(bitmap, address/page_size);
    }
}