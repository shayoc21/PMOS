//
// KERNEL--KERNEL.C
//
// 	The main unit of my kernel.
// 	Called directly by loader.c.
//
// 	Initialises the GDT, TSS, IDT, and enables paging.
//
// 	kernel_entry() will hand over to a user-space TTY once finished initialising everything.
//
#include "./include/vga_text.c"
/*
struct gdt_entry
{
	unsigned short limit_low;
	unsigned short base_low;
	unsigned char base_middle;
	unsigned char access;
	unsigned char flags_limit_low;
	unsigned char base_high;
} __attribute__((packed));
#define GDT_SELECTOR(idx, ring) ((idx << 3) | (ring & 0x3))
static struct gdt_entry gdt[6];
enum segments
{
	GDT_NULL = 0,
	GDT_KERNEL_CODE,
	GDT_KERNEL_DATA,
	GDT_USER_CODE,
	GDT_USER_DATA,
	GDT_TSS
};
void initialise_global_descriptor_table()
{
	gdt[GDT_NULL] = {0};
	gdt[GDT_KERNEL_CODE] =
	{
		.limit_low	= 0xFFFF;
		.base_low 	= 0x0000;
		.base_middle 	= 0x00;
		.access		= 0b10011010;
		.flags_limit_low= 0b11001111;
		.base_high	= 0x00;
	};
	gdt[GDT_KERNEL_DATA] =
	{
		.limit_low	= 0xFFFF;
		.base_low 	= 0x0000;
		.base_middle 	= 0x00;
		.access		= 0x10010010;
		.flags_limit_low= 0b11001111;
		.base_high	= 0x00;
	};
	gdt[GDT_USER_CODE] =
	{
		.limit_low	= 0xFFFF;
		.base_low 	= 0x0000;
		.base_middle 	= 0x00;
		.access		= 0b11111010;
		.flags_limit_low= 0x11001111;
		.base_high	= 0x00;
	};
	gdt[GDT_USER_DATA] =
	{
		.limit_low	= 0xFFFF;
		.base_low 	= 0x0000;
		.base_middle 	= 0x00;
		.access		= 0b11110010;
		.flags_limit_low= 0x11001111;
		.base_high	= 0x00;
	};
}

struct task_state_segment
{
	unsigned short previous_task_link;		unsigned short reserved0;

	unsigned int   esp0;
	unsigned short ss0;				unsigned short reserved1;

	unsigned int   esp1;
	unsigned short ss1;				unsigned short reserved2;

	unsigned int   esp2;
	unsigned short ss2;				unsigned short reserved3;
	
	unsigned int   cr3;
	unsigned int   eip;
	unsigned int   eflags;

	unsigned int   eax;
	unsigned int   ecx;
	unsigned int   edx;
	unsigned int   ebx;
	unsigned int   esp;
	unsigned int   ebp;
	unsigned int   esi;
	unsigned int   edi;

	unsigned short es;				unsigned short reserved4;
	unsigned short cs;				unsigned short reserved5;
	unsigned short ss;				unsigned short reserved6;
	unsigned short ds;				unsigned short reserved7;
	unsigned short fs;				unsigned short reserved8;
	unsigned short gs;				unsigned short reserved9;

	unsigned short ldt_selector;			unsigned short reserved10;

	unsigned short trap;
	unsigned short iomap_base;
};
static struct task_state_segment global_TSS = {0};

struct idt_entry
{
	unsigned short offset_low;
	unsigned short segment_selector;
	unsigned char reserved;
	unsigned char flags;
	unsigned short offset_high;
}; __attribute__((packed));
static struct idt[256];

void initialise_task_state_segment()
{
	unsigned long long base = (unsigned long long)&global_TSS;
	unsigned int limit = sizeof(global_TSS) - 1;

	gdt[GDT_TSS] = 
	{
		.limit_low = limit & 0xFFFF;
		.base_low = base & 0xFFFF;
		.base_middle = (base >> 16) & 0xFF;
		.access = 0b10001001 //present, ring 0, system segment, type 0x9 = 32 bit available T	
		.flags_limit_low = (limit >> 16) & 0xFFFF;
		.base_high = (base >> 24) & 0xFF;
	}
}
*/
__attribute__((section(".text.kernel_entry")))
void kernel_entry()
{

	vga_write_string("Paging works!!!!!!", 10, 0, TEXT_COLOUR_SUCCESS);

	for(;;){}
	return;
}

