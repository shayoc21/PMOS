//
//	kernel_head.c
//
//		Contains all code and functions to load the kernel from the disk into memory at 0x100000 (1MB)
//		Jumped to immediately from stage two -- so features debug/welcome messages
//		Limited includes as this file is stored in reserved sectors on disk
//		Enables a simple IDT to handle faults
//		Enables paging
//		Jumps into the kernel at virtual 0xC0000000
//

#include "./include/vga_text.c"
#include "./include/hardware_io.c"

static const char* kernel_file_name = "KERNEL  BIN";
static const int disk_number = 0x80;
static unsigned char sectors_per_cluster = 0x08;
static char* vga_text = (char*)0xB8000;

void read_disk_LBA(char* target, unsigned int LBA, unsigned char sector_count, const unsigned int disk_number);
void load_kernel();

typedef unsigned int page_entry;	//polymorphic type page_entry represents 4MB, 4KB directories and tables.. https://wiki.osdev.org/Paging#Page_Directory
static page_entry page_directory[1024] __attribute__((aligned(4096)));
static page_entry first_page_table[1024] __attribute__((aligned(4096)));
static page_entry kernel_page_table[1024] __attribute__((aligned(4096)));
static page_entry kernel_stack_pages[1024] __attribute__((aligned(4096)));

void initialise_paging()
{
	for (int i = 0; i < 1024; i++) { page_directory[i] = (page_entry)0x00000002; }
	//identity map first 1MB -- set supervisor so only ring 0 can write directly to vga, etc.
	for (int i = 0; i < 256; i++) { first_page_table[i] = (page_entry)((i * 0x1000) | 0b000000000011); }
	for (int i = 256; i < 1024; i++) { first_page_table[i] = (page_entry)0x0000002; }
	page_directory[0] = ((page_entry)first_page_table) | 0b000000000011;
	//the kernel could be placed in a PSE 4MB page, but that'd remove compatibility for i386 (and i'd have to recompile gcc which takes about 2 hours)
	for (int i = 0; i < 1024; i++) { kernel_page_table[i] = (0x100000 + (i * 0x1000)) | 0b000000000011; }
	page_directory[768] = ((page_entry)kernel_page_table) | 0b00000000011;
	//kernel stack will be 4 pages (16KB) and live just above the stack pages at 0xC0400000 -- SP would be 0xC07FFFFF
	for (int i = 0; i < 1020; i++) { kernel_stack_pages[i] = (page_entry)0x00000002; }
	for (int i = 1020; i < 1024; i++) { kernel_stack_pages[i] = (0x500000 + (i * 0x1000)) | 0b000000000011; } 
	page_directory[769] = ((page_entry)kernel_stack_pages) | 0b00000000011;
	unsigned int cr0;
	__asm__ volatile ("mov %0, %%cr3" : : "r"(page_directory));
	__asm__ volatile ("mov %%cr0, %0" :   "=r"(cr0));
	cr0 |= 0x80000000;
	__asm__ volatile ("mov %0, %%cr0" : : "r"(cr0));
}

__attribute__((section(".text.loader_entry")))
void loader_entry()
{
	vga_disable_cursor();
	const char* msg_loadingkernel = "[*] Loading Kernel..";
	vga_write_string(msg_loadingkernel, 6, 0, TEXT_COLOUR_PROCESS);

	load_kernel();
	initialise_paging();
	void (*kernel_entry)() = (void(*)())0xC0000000;
	kernel_entry();
	return;
}

//
// Loads the kernel directly into 0x100000, returns nothing
// This function will use 0x9000 as a placeholder address to store necessary FAT/Root sectors
//
void load_kernel()
{
	char* target = (char*)0x9000;
	char* kernel_target = (char*)0x100000;
	unsigned int kernel_cluster_size = sectors_per_cluster * 0x200;
	unsigned int cluster = 0x0000;

	// Load first sector of root directory. Guaranteed to store first cluster of the kernel.
	int root_directory_LBA = 0x1020;	//reserved sectors + spf * 2 + (2 - 2) * spc = 0x1020
	//reads the root directory first sector into 0x9000
	read_disk_LBA(target, root_directory_LBA, 1, disk_number);

	int kernel_found;
	for (unsigned int offset = 0; offset < 0x200; offset += 0x20)
	{
		kernel_found = 1;
		char* entry_file_name = (char*)(target + offset);
		for (int i = 0; i < 11; i++)
		{
			if (*(entry_file_name + i) != *(kernel_file_name + i)) { kernel_found = 0; }
		}
		if (kernel_found)
		{
			unsigned int low = *(unsigned short*)(target + offset + 26);
			unsigned int high = *(unsigned short*)(target + offset + 20);
			cluster = (high << 16) | low;
			break;
		}
	}
	if (!kernel_found)
	{
		const char* msg_error_finding_kernel = "[-] Error Finding Kernel..";
		vga_write_string(msg_error_finding_kernel, 7, 0, 0x04);
		return;
	}

	// cluster now stores the kernel first cluster
	int kernel_loaded = 0;
	int cluster_count = 0;
	int kernel_sector_LBA;
	while (!kernel_loaded)
	{
		unsigned int FAT_offset_bytes = cluster * 4;
		unsigned int FAT_entry_sector_LBA = 0x20 + (FAT_offset_bytes / 0x200);
		unsigned int FAT_entry_offset_in_sector = FAT_offset_bytes % 0x200;
		// reads the relevant sector of the FAT.. a bit wasteful as it rereads every loop, but space complexity O(1)
		read_disk_LBA(target, FAT_entry_sector_LBA, 1, disk_number);
	
		kernel_sector_LBA = 0x1020 + (cluster - 0x02) * sectors_per_cluster;
		read_disk_LBA(kernel_target + kernel_cluster_size * cluster_count, kernel_sector_LBA, sectors_per_cluster, disk_number);

		cluster = *(int*)(target + FAT_entry_offset_in_sector);
		cluster_count++;
		if (cluster >= 0x0FFFFFF8) { kernel_loaded = 1;}
	}
	const char* msg_kernel_loaded = "[+] Kernel Loaded..";
	vga_write_string(msg_kernel_loaded, 7, 0, TEXT_COLOUR_SUCCESS);
	return;
}

void read_disk_LBA(char* target, unsigned int LBA, unsigned char sector_count, const unsigned int disk_number)
{
	const char* read_error = "[-] Error Reading Disk..";
	const char* disk_fault = "[-] Disk Fault..";
	const char* test = ",,";
	const char* msg_timeout = "[-] Disk Read timeout..";
	unsigned short* buffer = (unsigned short*) target;

	for (unsigned char sector = 0; sector < sector_count; sector++)
	{

		unsigned char LBA0 = (LBA >> 0)  & 0xFF;
		unsigned char LBA1 = (LBA >> 8)  & 0xFF;
		unsigned char LBA2 = (LBA >> 16) & 0xFF;
		unsigned char LBA3 = (LBA >> 24) & 0x0F;
		unsigned char drive_head = (0xE0 | ((disk_number & 1) << 4) | LBA3);
		outb(0x1F2, 0x01);
		outb(0x1F3, LBA0); 
		outb(0x1F4, LBA1); 
		outb(0x1F5, LBA2);
		outb(0x1F6, drive_head);

		outb(0x1F7, 0x20);
		unsigned char status;

		unsigned int timeout = 1000000;
		while (timeout--)
		{
			status = inb(0x1F7);
			if (status & 0x80) continue;
			if (status & 0x01) break;
			if (status & 0x20) break;
			if (status & 0x08) break;
		}
		if (timeout == 0)
		{
			vga_write_string(msg_timeout, 7, 0, TEXT_COLOUR_FAILURE);
			return;
		}

		if (status & 0x01)
		{ 
			vga_write_string(read_error, 7, 0, TEXT_COLOUR_FAILURE);
			unsigned char err = inb(0x1F1);
			vga_write_hex((int)err, 7, 26*2, TEXT_COLOUR_FAILURE);
			vga_write_hex((int)status, 7, 30*2, TEXT_COLOUR_FAILURE);
			return;
		} 
		if (status & 0x20) { vga_write_string(disk_fault, 7, 0, 0x04); return; } // Drive fault bit
												  
		for (int i = 0; i < 256; i++) { buffer[sector*256 + i] = inw(0x1F0); }
		LBA++;
	}
	return;
}




	
