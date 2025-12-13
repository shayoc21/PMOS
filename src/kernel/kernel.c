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
#include "./interrupts/idt.h"

struct 
{
	u16_t limit;
	u32_t base;
} __attribute__((packed)) gdtr;
struct gdt_entry
{
	u16_t limit_low;
	u16_t base_low;
	u8_t base_middle;
	u8_t access;
	u8_t flags_limit_low;
	u8_t base_high;
} __attribute__((packed));
#define GDT_SELECTOR(idx, ring) ((idx << 3) | (ring & 0x3))
typedef u16_t gdt_selector;
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
	gdt[GDT_NULL] = (struct gdt_entry){0};
	gdt[GDT_KERNEL_CODE] = (struct gdt_entry)
	{
		0xFFFF,		// limit_low
		0x0000,		// base_low
		0x00,		// base_middle
		0b10011010,	// access
		0b11001111,	// flags_limit_low
		0x00		// base_high
	};

	gdt[GDT_KERNEL_DATA] = (struct gdt_entry)
	{
		0xFFFF,		// limit_low
		0x0000,		// base_low
		0x00,		// base_middle
		0b10010010,	// access
		0b11001111,	// flags_limit_low
		0x00		// base_high
	};

	gdt[GDT_USER_CODE] = (struct gdt_entry)
	{
		0xFFFF,		// limit_low
		0x0000,		// base_low
		0x00,		// base_middle
		0b11111010,	// access
		0b11001111,	// flags_limit_low
		0x00		// base_high
	};

	gdt[GDT_USER_DATA] = (struct gdt_entry)
	{
		0xFFFF,		// limit_low
		0x0000,		// base_low
		0x00,		// base_middle
		0b11110010,	// access
		0b11001111,	// flags_limit_low
		0x00		// base_high
	};
	gdtr.limit = sizeof(gdt) - 1;
	gdtr.base = (u32_t)&gdt;
	__asm__ volatile("lgdt %0" : : "m"(gdtr));
}

struct task_state_segment
{
	u16_t previous_task_link; u16_t	reserved0;

	u32_t   esp0;
	u16_t ss0;		  u16_t reserved1;

	u32_t   esp1;
	u16_t ss1;		  u16_t reserved2;

	u32_t   esp2;
	u16_t ss2;		  u16_t reserved3;
	
	u32_t   cr3;
	u32_t   eip;
	u32_t   eflags;

	u32_t   eax;
	u32_t   ecx;
	u32_t   edx;
	u32_t   ebx;
	u32_t   esp;
	u32_t   ebp;
	u32_t   esi;
	u32_t   edi;

	u16_t es;		  u16_t reserved4;
	u16_t cs;		  u16_t reserved5;
	u16_t ss;		  u16_t reserved6;
	u16_t ds;		  u16_t reserved7;
	u16_t fs;		  u16_t reserved8;
	u16_t gs;		  u16_t reserved9;

	u16_t ldt_selector;	  u16_t reserved10;

	u16_t trap;
	u16_t iomap_base;
};
static struct task_state_segment global_TSS = {0};


void call_interrupt(u8_t vector)
{
}

void initialise_task_state_segment(gdt_selector ss_selector, u32_t sp)
{
	global_TSS.esp0 = sp;
	global_TSS.ss0 = ss_selector;
	u32_t base = (u32_t)&global_TSS;
	u32_t limit = sizeof(global_TSS) - 1;

	gdt[GDT_TSS] = (struct gdt_entry)
	{
		(u16_t)limit & 0xFFFF,
		(u16_t)base & 0xFFFF,
		(u8_t)(base >> 16) & 0xFF,
		(u8_t)0b10001001, //present, ring 0, system segment, type 0x9 = 32 bit available T	
		(u8_t)((0b1100 << 4) | ((limit >> 16) & 0x0F)),
		(u8_t)((base >> 24) & 0xFF)
	};
	__asm__ volatile ("ltr %0" : : "r"(GDT_SELECTOR(GDT_TSS, 0)));
}

__attribute__((section(".text.kernel_entry")))
void kernel_entry()
{
	initialise_global_descriptor_table();

	u16_t kernel_seg = GDT_SELECTOR(GDT_KERNEL_DATA, 0);
	__asm__ volatile
	(
		"mov %0, %%ax\n\t"
		"mov %%ax, %%ds\n\t"
		"mov %%ax, %%es\n\t"
		"mov %%ax, %%fs\n\t"
		"mov %%ax, %%gs\n\t"
		: : "r"(kernel_seg) : "ax"
	);

	gdt_selector ss_selector = GDT_SELECTOR(2, 0);
	//as in kernel_head.c, the sp will point to the top of page directory 769, mapping 0xC0400000->0xC07FFFFF. the stack has 4 pages for 16KB
	u32_t sp = 0xC07FFFFF;

	__asm__ volatile
	(
		"cli\n\t"
		"mov %0, %%ax\n\t"
		"mov %%ax, %%ss\n\t"
		"mov %1, %%esp\n\t"
		:
		: "r"(ss_selector), "r"(sp)
		: "ax"
	);

	initialise_task_state_segment(ss_selector, sp);
	initialise_idt();


	vga_write_string("Below is 'test isr' and 'Divide by Zero'. If both print then ISR works", 15,0 , TEXT_COLOUR_SUCCESS);


	//CALLING TEST INTERRUPT 0X90.. PRINTS A YELLOW "test isr" MESSAGE
	__asm__ volatile ( "int %0" : : "i"(0x90));

	for(;;){}
	return;
}

