#ifndef GDT_IDT_H
#define GDT_IDT_H

#include <stdint.h>

struct gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access_byte;
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed));

struct gdt_entry gdt[3];

struct lgdt {
    uint16_t size;
    uint64_t address;
} __attribute__((packed));

struct lgdt gdt_ptr;

void set_gdt_entry(int index, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags) {
    gdt[index].base_low = base;
    gdt[index].base_middle = (base >> 16) & 0xFF;
    gdt[index].base_high = base >> 24;

    gdt[index].limit_low = limit;
    gdt[index].access_byte = access;

    gdt[index].granularity = (flags & 0xF0) | ((limit >> 16) & 0x0F);
}
extern void load_gdt(void *ptr);

struct idt_entry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t ist;
    uint8_t type_attr;
    uint16_t offset_middle;
    uint32_t offset_high;
    uint32_t zero;
} __attribute__((packed));

struct idtr {
    uint16_t size;
    uint64_t address;
} __attribute__((packed));

struct idt_entry idt[256];
struct idtr idt_ptr;

void set_idt_entry(int index, uint64_t offset, uint16_t selector, uint8_t type_attr, uint8_t ist) {
    idt[index].offset_low = offset & 0xFFFF;
    idt[index].selector = selector;
    idt[index].ist = ist & 0x07;
    idt[index].type_attr = type_attr;
    idt[index].offset_middle = (offset >> 16) & 0xFFFF;
    idt[index].offset_high = (offset >> 32) & 0xFFFFFFFF;
    idt[index].zero = 0;
}

extern void load_idt(void *ptr);
#endif