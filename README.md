# WheatOS 🌾

An Operating System based on x86 for computers, built from scratch to explore low-level hardware interactions and kernel development. 

**Belive it or not, it's an OS.** Built with Assembly and C. No Linux. 🧑‍💻

## 💡 Why WheatOS?
* **Passion Project:** Created to deeply understand how computers actually work under the hood.
* **Skill Building:** Teaches low-level programming, x86 architecture, and the fundamentals of Operating Systems in an expressive way.
* **Future Vision:** The system might have A.I. integrated later to answer questions about the OS, chat about daily life, or make changes to device files.

## 🚀 Current Progress & Features

WheatOS has successfully transitioned from 16-bit Real Mode all the way to a 64-bit Long Mode preemptive kernel. 

**Core Architecture:**
* Custom Bootloader (MBR, Stage 2, A20 line, E820 memory map, GDT, 2MB Paging, Long Mode).
* Cross-compiler toolchain and custom linker script.
* Automated build script (`build.sh`) with dynamic sector count patching.

**Kernel & Memory Management:**
* **VGA Text Driver:** Printing, numbers, cursor, backspace, scrolling, and arrow keys.
* **Interrupt Handling:** IDT setup, PIC remap, IRQ dispatcher, and 32 exception handlers with a custom Kernel Panic screen.
* **Memory Management:** E820 parsing, Physical Memory Manager (PMM) with `alloc_page`/`free_page`, and Virtual Memory Paging (`map_page`, `unmap_page`).
* **Kernel Heap:** Custom `kmalloc` and `kfree` implementations.

**Hardware Drivers:**
* PS/2 Keyboard driver (with Shift key support).
* Programmable Interval Timer (PIT) running at 100 Hz with custom `sleep` functionality.

**Process Management (In Progress):**
* Multitasking (Cooperative and Preemptive scheduling).
* Task lifecycle management (`task_exit` and `reap_dead_tasks`).
* User Mode preparations (Ring 3 GDT entries, 64-bit TSS, and `enter_user` iretq transitions).

## 🗺️ Roadmap & Next Steps

* [ ] **Ring 3 Execution:** Fix page faults by setting user bits across all paging levels.
* [ ] **System Calls:** Implement `int 0x80` syscall dispatcher (starting with `sys_print`).
* [ ] **Memory Protection:** Separate user pages from the kernel's 1GB identity map and validate user pointers.
* [ ] **User Tasks:** Run user code as fully scheduled, preemptable tasks.
* [ ] **Storage:** Write IDE/ATA sector drivers and a basic filesystem (FAT16/Ext2).
* [ ] **Program Loading:** Parse and execute flat binaries or ELF files from disk.
* [ ] **Userland Shell:** Build an interactive command-line interface.

## 🛠️ Repository Structure

* `asm_lib/` - Low-level assembly routines (GDT, IDT, Bootloader, Context Switching).
* `c_lib/` - Kernel C library and subsystem headers (Memory, Multitasking, Hardware drivers).
* `kernel.c` - Main kernel entry point.
* `mbr.asm` / `stage2.asm` - Bootloader stages.
* `build.sh` - Automated build script for compiling and generating `disk.img`.

## ⚙️ Building the OS

*Ensure you have an `x86_64-elf` cross-compiler (GCC & Binutils), NASM, and QEMU installed.*

To build and compile the OS image, run the build script:
```bash
./build.sh
```
This will compile the C and Assembly files, link them into `kernel.bin`, generate the bootloader binaries, and stitch them together into `disk.img`. You can then boot `disk.img` using QEMU.