This is a big project, so I will use this text file to track progress and plan.

# Documentation

---
#### Building:

Only tested in my development environment on Linux, though the code is 386 specific and should run on any emulator.

I am using qemu for i386, with mtools to help with disk partitioning/writing in building.

To build, ensure 'build' is an executable script, and run ./build

To run, do the above but with ./run

---

#### Debugging:

My debugging environment is included with the repository, to allow easier portable development. 

I am using bochs, with ndisasm to disassemble

----

# Development

----
## v0.0

Version 0.0 is the first stable booting version of my operating system.
It should boot from a floppy disk in fat12, load stage 2 into memory, put the system into protected mode, and finally read the kernel from the hard disk into memory.

#### v0.0.1

Stage one bootloader, loads stage two into 0x0050:0x0000
stage two just prints a welcome message for now.

#### v0.0.2

Stage two bootloader 'finished'. Temporary GDT with code and data segment.
A20 enabled, computer booted into protected mode.

#### v0.0.3

Completely rewritten stage one bootloader, now with FAT32.
Loads stage two from a reserved sector instead of saving it as a file.
Does use extended read now, if I ever intend to port to older hardware I'll have to add CHS conversion.
