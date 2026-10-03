#include <inttypes.h>
#include <stddef.h>
#define page_size 0x200000ULL

uint8_t x = 0;
uint8_t y = 0;
char buffer[21];
uint8_t bitmap[1024];

extern void load_gdt(void *ptr);
extern uint64_t isr_table[32];

char *int_to_str(uint64_t number) {
    if (number == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
    }
    //find length
    uint64_t floor_num = 10;
    int i = 1;
    while(number / floor_num != 0) {
        i += 1;
        floor_num *= 10;
    }
    int length = i+1;

    floor_num = 1;
    uint64_t mod_num = 10;
    buffer[length - 1] = '\0';
    //find hex value
    for (int i = length-2; i >= 0; i--) {
        buffer[i] = (char)(number % mod_num / floor_num) + 48;
        floor_num *= 10;
        mod_num *= 10;
    }
    return buffer;
}

void write_character(unsigned char c, unsigned char forecolor, unsigned char backcolor) {
    if(c == '\n' || x >= 80) {
        x = 0;
        y += 1;
        if (c == '\n') {
            return;
        }
    }
    uint16_t attrib = (backcolor << 4) | (forecolor & 0x0F);
    volatile uint16_t * where;
    where = (volatile uint16_t *)0xB8000 + (y * 80 + x);
    *where = c | (attrib << 8);
    x += 1;
}

void print(const char *string) {
    while (*string) {
        write_character(*string, 7, 0);
        string++;
    }
}

void clear_screen() {
    x = y = 0;
    for(int i = 0; i < (80*25); i++){
        write_character(' ', 7, 0);
    }
    x = y = 0;
}
void new_line() {
    write_character('\n', 7, 0);
}

struct e820_entry {
    uint64_t base_address;
    uint64_t length;
    uint32_t type;
    uint32_t extended_attribute;
} __attribute__((packed));

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
extern void default_exception_handler(void);
void exception_handler_c(uint64_t vector_number, uint64_t error_code) {
    clear_screen();
    print("--- WHEAT KERNEL PANIC ---\n");
    print("Exception Vector: ");
    print(int_to_str(vector_number)); new_line();
    print("Error Code: ");
    print(int_to_str(error_code)); new_line();
    print("System Halted.");
    while (1) {
        __asm__ volatile ("cli; hlt");
    }
}

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

void kernel_main(void *e820_map, int entry_count) {
    clear_screen();
    print("hello world\n\n");
    
    struct e820_entry *entries = (struct e820_entry *)e820_map;

    for (int i = 0; i < entry_count; i++) {
        print(int_to_str(entries[i].base_address));
        print(" ");
    }

    new_line();

    for (int i = 0; i < entry_count; i++) {
        print(int_to_str(entries[i].length));
        print(" ");
    }

    new_line();

    for (int i = 0; i < entry_count; i++) {
        print(int_to_str(entries[i].type));
        print(" ");
    }

    //find the memory map information
    uint64_t highest_address = 0;
    for (int i = 0; i < entry_count; i++) {
        if (entries[i].type != 1) {
            continue;
        }
        uint64_t end_address = entries[i].base_address + entries[i].length;
        if (end_address > highest_address) {
            highest_address = end_address;
        }
    }
    uint32_t total_pages = highest_address / page_size;

    //go through the bitmap
    uint32_t needed_bytes = (total_pages + 7) / 8;

    for (int i = 0; i < total_pages; i++) {
        uint64_t page_address = (uint64_t)i * page_size;
        bool found = false;

        for (int j = 0; j < entry_count; j++) {
            if ((entries[j].base_address + entries[j].length) > page_address && page_address >= entries[j].base_address) {
                if (entries[j].type == 1) {
                    clear_bit(bitmap, i);
                } else {
                    set_bit(bitmap, i);
                }

                found = true;
            }
        }
        if (!found) {
            set_bit(bitmap, i);
        }
    }

//reserve kernel memory

    //reserve stack memory
    uint32_t stack_top = 0x800000;
    uint32_t stack_bottom = stack_top - 0x10000;

    uint32_t start_page = stack_bottom / page_size;
    uint32_t end_page = (stack_top + page_size - 1) / page_size;

    for (int i = start_page; i < end_page; i++) {
        set_bit(bitmap, i);
    }

    //reserve e820 memory
    uint32_t e820_buffer_start = (uint32_t)(uintptr_t)e820_map;
    uint32_t e820_buffer_end = e820_buffer_start + 768;

    start_page = e820_buffer_start / page_size;
    end_page = (e820_buffer_end + page_size - 1) / page_size;

    for (int i = start_page; i < end_page; i++) {
        set_bit(bitmap, i);
    }

    // kernel code/data range
    extern uint8_t _bss_end;

    uint32_t kernel_start = 0xD000;
    uint32_t kernel_end = (uint32_t)(uintptr_t)&_bss_end;

    start_page = kernel_start / page_size;
    end_page = (kernel_end + page_size - 1) / page_size;
    
    for (int i = start_page; i < end_page; i++) {
        set_bit(bitmap, i);
    }

    set_gdt_entry(0, 0, 0, 0, 0);
    // gdt for kernel code
    set_gdt_entry(1, 0, 0, 0x9A, 0x20);
    // gdt for kernel data
    set_gdt_entry(2, 0, 0, 0x92, 0);

    gdt_ptr.size = (sizeof(struct gdt_entry) * 3) - 1;
    gdt_ptr.address = (uint64_t)(uintptr_t)&gdt;

    load_gdt(&gdt_ptr);

    // Set up IDT
    idt_ptr.size = (sizeof(struct idt_entry) * 256) - 1;
    idt_ptr.address = (uint64_t)(uintptr_t)&idt;


    for (int i = 0; i < 256; i++) {
        if (i < 32) {
            set_idt_entry(i, isr_table[i], 0x08, 0x8E, 0);
        } else {
            set_idt_entry(i, (uint64_t)(uintptr_t)default_exception_handler, 0x08, 0x8E, 0);
        }
    }


    load_idt(&idt_ptr);

    new_line();
    print("Kernel initialized successfully!\n");
    print("Triggering intentional divide-by-zero test...\n");

    //__asm__ volatile ("ud2");

    while (1) {}
}