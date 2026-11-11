# WheatOS 🌾

An Operating System based on x86 for computers, built from scratch to explore low-level hardware interactions and kernel development. 

**Belive it or not, it's an OS.** Built with Assembly and C. No Linux.

## What Is Special About It?

WheatOS has A.I. integrated, and it is **offline**, not online. Even without internet, the A.I. still going to work.

## Sadly

WheatOS is not fully developed yet.

- A.I. Integrated
- Offcial Releases

These things are not fully developed yet. However, you can view the source and the progress, what I actually did for this project. Right now it only run on a virtual machine.

## What I Actually Did?

- An Operating System (OS) for computers with x86 CPU
- A basic kernel
- A basic shell program run from kernel

## How Do You Use It?

### **Requirement:**

- You will need to clone this repo to your device
- `Linux machine` or a `WSL` to build the files.

### **Require Tools:**

- The Netwide Assembler (`NASM`) will need in order to compile the `.asm` files.

```bash
sudo apt install nasm
```

- `GCC` is the compiler for C files.

```bash
sudo apt install gcc
```

- `QEMU` is a virtual machine that can help you run the `WheatOS`.

```bash
sudo apt install qemu-system-x86
```

When you have those things ready, you are ready to run the `build.sh` file and it will do the rest for you.

```bash
./build.sh
```

### Saving files

Inside WheatOS, use `write notes hello` to create a file with text, `cat notes`
to read it, and `ls` to list files. `touch` creates an empty file and `rm` deletes
a file. Changes are automatically saved by the kernel to `build/fs.img`.

You can stop QEMU and run `./build.sh` again; the script preserves `build/fs.img`
and WheatOS loads your files at startup. Keep this image when cleaning or moving
the build directory, since deleting it deletes your saved files. If saving fails,
the shell reports that the change exists only in memory.
