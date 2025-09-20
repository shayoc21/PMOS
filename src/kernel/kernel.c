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

typedef unsigned char u8_t;
typedef unsigned short u16_t;
typedef unsigned int u32_t;
typedef char i8_t;
typedef short i16_t;
typedef int i32_t;

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

struct idt_entry
{
	u16_t offset_low;
	u16_t segment_selector;
	u8_t reserved;
	u8_t flags;
	u16_t offset_high;
}; __attribute__((packed));
static struct idt[256];
struct interrupt
{
	void(*isr_ptr)(void);
	u8_t flags;
};
#define __ISR__ __attribute__((interrupt))
#define INTERRUPT_TASK
#define INTERRUPT_TRAP
#define INTERRUPT_FAULT
#define INTERRUPT_ABORT
#define INTERRUPT_NONTRAPPING
__ISR__ void unhandled_interrupt(void* frame)
{
    vga_write_string("INTERRUPT NOT HANDLED", 24, 3, TEXT_COLOUR_FAILURE);
}

// CPU exceptions
__ISR__ void int_0_divide_error(void* frame)
{
	vga_write_string("(INT.0) DIVIDE ERROR.", 24, 3, TEXT_COLOUR_FAILURE);
}
__ISR__ void int_1_debug_exception(void* frame)
{
	vga_write_string("(INT.1) DEBUG EXCEPTION.", 24, 3, TEXT_COLOUR_FAILURE);
}
__ISR__ void int_2_nmi_exception(void* frame)
{
	vga_write_string("(INT.2) NMI EXCEPTION.", 24, 3, TEXT_COLOUR_FAILURE);
}
__ISR__ void int_3_breakpoint(void* frame)
{
	vga_write_string("(INT.3) BREAKPOINT.", 24, 3, TEXT_COLOUR_FAILURE);
}
__ISR__ void int_4_overflow(void* frame)
{
	vga_write_string("(INT.4) OVERFLOW.", 24, 3, TEXT_COLOUR_FAILURE);
}
__ISR__ void int_5_bound_range_exceeded(void* frame)
{
	vga_write_string("(INT.5) BOUND RANGE EXCEEDED.", 24, 3, TEXT_COLOUR_FAILURE);
}
__ISR__ void int_6_invalid_opcode(void* frame)
{
	vga_write_string("(INT.6) INVALID OPCODE.", 24, 3, TEXT_COLOUR_FAILURE);
}
__ISR__ void int_7_device_not_available(void* frame)
{
	vga_write_string("(INT.7) DEVICE NOT AVAILABLE.", 24, 3, TEXT_COLOUR_FAILURE);
}
__ISR__ void int_8_double_fault(void* frame)
{
	vga_write_string("(INT.8) DOUBLE FAULT.", 24, 3, TEXT_COLOUR_FAILURE);
}
__ISR__ void int_9_coprocessor_segment_overrun(void* frame)
{
	vga_write_string("(INT.9) COPROCESSOR SEGMENT OVERRUN.", 24, 3, TEXT_COLOUR_FAILURE);
}
__ISR__ void int_10_invalid_tss(void* frame)
{
	vga_write_string("(INT.10) INVALID TSS.", 24, 3, TEXT_COLOUR_FAILURE);
}
__ISR__ void int_11_segment_not_present(void* frame)
{
	vga_write_string("(INT.11) SEGMENT NOT PRESENT.", 24, 3, TEXT_COLOUR_FAILURE);
}
__ISR__ void int_12_stack_segment_fault(void* frame)
{
	vga_write_string("(INT.12) STACK SEGMENT FAULT.", 24, 3, TEXT_COLOUR_FAILURE);
}
__ISR__ void int_13_general_protection_fault(void* frame)
{
	vga_write_string("(INT.13) GENERAL PROTECTION FAULT.", 24, 3, TEXT_COLOUR_FAILURE);
}
__ISR__ void int_14_page_fault(void* frame)
{
	vga_write_string("(INT.14) PAGE FAULT.", 24, 3, TEXT_COLOUR_FAILURE);
}
__ISR__ void int_15_reserved(void* frame)
{
	vga_write_string("(INT.15) RESERVED.", 24, 3, TEXT_COLOUR_FAILURE);
}
__ISR__ void int_16_x87_fpu_floating_point_error(void* frame)
{
	vga_write_string("(INT.16) X87 FPU FLOATING POINT ERROR.", 24, 3, TEXT_COLOUR_FAILURE);
}
__ISR__ void int_17_alignment_check(void* frame)
{
	vga_write_string("(INT.17) ALIGNMENT CHECK.", 24, 3, TEXT_COLOUR_FAILURE);
}
__ISR__ void int_18_machine_check(void* frame)
{
	vga_write_string("(INT.18) MACHINE CHECK.", 24, 3, TEXT_COLOUR_FAILURE);
}
__ISR__ void int_19_simd_floating_point_exception(void* frame)
{
	vga_write_string("(INT.19) SIMD FLOATING POINT EXCEPTION.", 24, 3, TEXT_COLOUR_FAILURE);
}
__ISR__ void int_20_virtualisation_exception(void* frame)
{
	vga_write_string("(INT.20) VIRTUALISATION EXCEPTION.", 24, 3, TEXT_COLOUR_FAILURE);
}
__ISR__ void int_21_control_protection_exception(void* frame)
{
	vga_write_string("(INT.21) CONTROL PROTECTION EXCEPTION.", 24, 3, TEXT_COLOUR_FAILURE);
}

void initialise_interrupt_descriptor_table()
{
	//icw1
	outb(0x0020, 0x11);
	outb(0x00A0, 0x11);
	//icw2
	outb(0x0021, 0x20);
	outb(0x00A1, 0x28);
	//icw3
	outb(0x0021, 0x04);
	outb(0x00A1, 0x02);
	//icw4
	outb(0x0021, 0x01);
	oubt(0x00A1, 0x01);

	struct interrupt cpu_exceptions[22] = 
	{
		(struct interrupt){&int_0_divide_error, (0b1000 | INTERRUPT_FAULT)},
		(struct interrupt){&int_1_debug_exception, (0b1000 | INTERRUPT_TRAP)},
		(struct interrupt){&int_2_nmi_exception, (0b1000 | INTERRUPT_NONTRAPPING)},
		(struct interrupt){&int_3_breakpoint, (0b1000 | INTERRUPT_TRAP)},
		(struct interrupt){&int_4_overflow, (0b1000 | INTERRUPT_TRAP)},
		(struct interrupt){&int_5_bound_range_exceeded, (0b1000 | INTERRUPT_FAULT)},
		(struct interrupt){&int_6_invalid_opcode, (0b1000 | INTERRUPT_FAULT)},
		(struct interrupt){&int_7_device_not_available, (0b1000 | INTERRUPT_FAULT)},
		(struct interrupt){&int_8_double_fault, (0b1000 | INTERRUPT_ABORT)},
		(struct interrupt){&int_9_coprocessor_segment_overrun, (0b1000 | INTERRUPT_FAULT)},
		(struct interrupt){&int_10_invalid_tss, (0b1000 | INTERRUPT_FAULT)},
		(struct interrupt){&int_11_segment_not_present, (0b1000 | INTERRUPT_FAULT)},
		(struct interrupt){&int_12_stack_segment_fault, (0b1000 | INTERRUPT_FAULT)},
		(struct interrupt){&int_13_general_protection_fault, (0b1000 | INTERRUPT_FAULT)},
		(struct interrupt){&int_14_page_fault, (0b1000 | INTERRUPT_FAULT)},
		(struct interrupt){&int_15_reserved, (0b1000 | INTERRUPT_FAULT)},
		(struct interrupt){&int_16_x87_fpu_floating_point_error, (0b1000 | INTERRUPT_FAULT)},
		(struct interrupt){&int_17_alignment_check, (0b1000 | INTERRUPT_FAULT)},
		(struct interrupt){&int_18_machine_check, (0b1000 | INTERRUPT_ABORT)},
		(struct interrupt){&int_19_simd_floating_point_exception, (0b1000 | INTERRUPT_FAULT)},
		(struct interrupt){&int_20_virtualisation_exception, (0b1000 | INTERRUPT_FAULT)},
		(struct interrupt){&int_21_control_protection_exception, (0b1000 | INTERRUPT_FAULT)}
	};	

	for (i32_t i = 0; i < 22; i++)
	{
		struct interrupt exception_i = cpu_exceptions[i];
		idt[i] = 
		{
			.offset_low = (u16_t)(exception_i.isr_ptr & 0xFFFF);
			.segment_selector = (u8_t)GDT_SELECTOR(1, 0);
			.reserved = (u8_t)0x00;
			.flags = (u8_t)exception_i.flags;
			.offset_high = (u16_t)((exception_i.isr_ptr >> 16) * 0xFFFF);
		};
	}


	}
	//fill idt with "unhandled" isr
	void (*unhandled_isr_ptr)(void*) = &unhandled_interrupt;
	for (i32_t i = 22; i < 256; i++)
	{
		idt[i] = 
		{
			.offset_low = (u16_t)(unhandled_isr_ptr & 0xFFFF);
			.segment_selector = (u8_t)GDT_SELECTOR(1, 0);
			.reserved = (u8_t)0x00;
			.flags = 0b11101110;
			.offset_high = (u16_t)((unhandled_isr_ptr >> 16) & 0xFFFF);
		};
	}
}

void call_interrupt(u8_t vector)
{
	__asm__ __volatile__ ( "int %0" : : "i"(vector));
}

void initialise_task_state_segment(gdt_selector ss_selector, u32_t sp)
{
	global_TSS.esp0 = sp;
	global_TSS.ss0 = ss_selector;
	unsigned long long base = (unsigned long long)&global_TSS;
	u32_t limit = sizeof(global_TSS) - 1;

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

__attribute__((section(".text.kernel_entry")))
void kernel_entry()
{
	initialise_global_descriptor_table();
	gdt_selector ss_selector = GDT_SELECTOR(2, 0);
	//as in kernel_head.c, the sp will poi32_tto the top of page directory 769, mapping 0xC0400000->0xC07FFFFF. the stack has 4 pages for 16KB
	u32_t sp = 0xC07FFFFF;
	__asm__ volatile
	(
	 	"cli\n\t"
		"mov %0, %%ax\n\t"
		"mov %%ax, %%ss\n\t"
		"mov %1, %%eax\n\t"
		"mov %%eax, %%sp\n\t"
		:
		: "r"(ss_selector), "r"(sp)
		: "eax"
	)
	initialise_task_state_segment(ss_selector, sp);
	initialise_interrupt_descriptor_table();

	call_interrupt(0x90);

	for(;;){}
	return;
}

