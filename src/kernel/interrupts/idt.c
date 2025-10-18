typedef unsigned char u8_t;
typedef unsigned short u16_t;
typedef unsigned int u32_t;
typedef char i8_t;
typedef short i16_t;
typedef int i32_t;

#include "../include/vga_text.h"

extern void divide_by_zero		(void);
extern void debug_exception		(void);
extern void non_maskable_interrupt	(void);
extern void breakpoint			(void);
extern void overflow			(void);
extern void bound_range_exceeded	(void);
extern void invalid_opcode		(void);
extern void device_not_available	(void);
extern void double_fault		(void);
extern void coprocessor_segment_overrun	(void);
extern void invalid_tss			(void);
extern void segment_not_present		(void);
extern void stack_segment_fault		(void);
extern void general_protection_fault	(void);
extern void page_fault			(void);
extern void floating_point_error	(void);
extern void alignment_check		(void);
extern void machine_check		(void);
extern void simd_floating_point		(void);
extern void virtualization_exception	(void);
extern void control_protection		(void);

extern void isr_test(void);

struct idt_entry
{
	u16_t offset_low;
	u16_t segment_selector;
	u8_t reserved;
	u8_t flags;
	u16_t offset_high;
} __attribute__((packed));
static struct idt_entry idt[256];

void create_interrupt(u8_t vector, void (*isr)(void), u16_t segment, u8_t flags)
{
	idt[vector] = (struct idt_entry)
	{
		(u16_t)((u32_t)isr & 0xFFFF),
		segment,
		0x00,
		flags,
		(u16_t)(((u32_t)isr >> 16) & 0xFFFF)
	};
}

#define KERNEL_CODE_SEG 0x08
#define INTERRUPT 0x0E

void initialise_idt()
{
	//reprogram the PIC

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
	outb(0x00A1, 0x01);

	create_interrupt( 0, divide_by_zero,       		KERNEL_CODE_SEG, INTERRUPT);
	create_interrupt( 1, debug_exception,           	KERNEL_CODE_SEG, INTERRUPT);
	create_interrupt( 2, non_maskable_interrupt,     	KERNEL_CODE_SEG, INTERRUPT);
	create_interrupt( 3, breakpoint,                 	KERNEL_CODE_SEG, INTERRUPT);
	create_interrupt( 4, overflow,                   	KERNEL_CODE_SEG, INTERRUPT);
	create_interrupt( 5, bound_range_exceeded,       	KERNEL_CODE_SEG, INTERRUPT);
	create_interrupt( 6, invalid_opcode,             	KERNEL_CODE_SEG, INTERRUPT);
	create_interrupt( 7, device_not_available,       	KERNEL_CODE_SEG, INTERRUPT);
	create_interrupt( 8, double_fault,               	KERNEL_CODE_SEG, INTERRUPT);
	create_interrupt( 9, coprocessor_segment_overrun,	KERNEL_CODE_SEG, INTERRUPT);
	create_interrupt(10, invalid_tss,                	KERNEL_CODE_SEG, INTERRUPT);
	create_interrupt(11, segment_not_present,        	KERNEL_CODE_SEG, INTERRUPT);
	create_interrupt(12, stack_segment_fault,        	KERNEL_CODE_SEG, INTERRUPT);
	create_interrupt(13, general_protection_fault,   	KERNEL_CODE_SEG, INTERRUPT);
	create_interrupt(14, page_fault,                 	KERNEL_CODE_SEG, INTERRUPT);
	create_interrupt(16, floating_point_error,       	KERNEL_CODE_SEG, INTERRUPT);
	create_interrupt(17, alignment_check,           	KERNEL_CODE_SEG, INTERRUPT);
	create_interrupt(18, machine_check,             	KERNEL_CODE_SEG, INTERRUPT);
	create_interrupt(19, simd_floating_point,       	KERNEL_CODE_SEG, INTERRUPT);
	create_interrupt(20, virtualization_exception,  	KERNEL_CODE_SEG, INTERRUPT);
	create_interrupt(21, control_protection,        	KERNEL_CODE_SEG, INTERRUPT);

	create_interrupt(0x90, isr_test, KERNEL_CODE_SEG, INTERRUPT);
}


