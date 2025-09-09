This is a big project, so I will use this text file to track progress and plan.

# Documentation

--


# Development

## v0.1

Version 0.1 is the first stable booting version of my operating system.
It should boot from a floppy disk in fat12, load stage 2 into memory, put the system into protected mode, and finally read the kernel from the hard disk into memory.

### v0.1.0

Stage one bootloader, loads stage two into 0x0050:0x0000
stage two just prints a welcome message for now.

