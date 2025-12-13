
#include "../include/vga_text.h"

//
//	Stack:
//		gs, fs, es, ds
//		edi, esi, ebp, esp, ebx, edx, ecx, eax
//		error code
//		handler address
//		eip
//		cs
//		eflags
//
//		(WHEN PRIVILEGE LEVEL CHANGES)
//		user esp
//		user ss
//
struct isr_stack_frame
{
	unsigned int gs, fs, es, ds;
	unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
	unsigned int err_code;
	unsigned int handler_address;	//can be ignored
	unsigned int eip;
	unsigned int cs;
	unsigned int eflags;
	unsigned int user_esp, user_ss;
};

//
//	CPU exception handlers
//
//		If triggered from user space, gives the scheduler a signal that the current running process has caused a fault
//
//		If triggered from kernel space, initiates a system panic
//
//


//temporary definitions
void scheduler_signal_error(const char* a)
{
	vga_write_string(a, 20, 20, TEXT_COLOUR_FAILURE);
}
void system_panic(const char* a) {scheduler_signal_error(a); for(;;){}}

#define DEFINE_EXCEPTION(_name,_msg) 							\
void _name(struct isr_stack_frame frame)						\
{											\
	scheduler_signal_error(_msg);						\
	if ((frame.cs & 3) == 0) 							\
       	{										\
		system_panic(_msg);							\
	}										\
}											
DEFINE_EXCEPTION(divide_by_zero_handler, "Divide by Zero")
DEFINE_EXCEPTION(debug_exception_handler, "Debug Exception")
DEFINE_EXCEPTION(nmi_handler, "Non-Maskable Interrupt")
DEFINE_EXCEPTION(breakpoint_handler, "Breakpoint")
DEFINE_EXCEPTION(overflow_handler, "Overflow")
DEFINE_EXCEPTION(bound_handler, "BOUND Range Exceeded")
DEFINE_EXCEPTION(invalid_opcode_handler, "Invalid Opcode")
DEFINE_EXCEPTION(device_not_available_handler, "Device Not Available")
DEFINE_EXCEPTION(double_fault_handler, "Double Fault")
DEFINE_EXCEPTION(coprocessor_handler, "Coprocessor Segment Overrun")
DEFINE_EXCEPTION(invalid_tss_handler, "Invalid TSS")
DEFINE_EXCEPTION(segment_not_present_handler, "Segment Not Present")
DEFINE_EXCEPTION(stack_segment_fault_handler, "Stack Segment Fault")
DEFINE_EXCEPTION(general_protection_fault_handler, "General Protection Fault")
DEFINE_EXCEPTION(page_fault_handler, "Page Fault")
DEFINE_EXCEPTION(floating_point_handler, "x87 Floating Point Exception")
DEFINE_EXCEPTION(alignment_check_handler, "Alignment Check")
DEFINE_EXCEPTION(machine_check_handler, "Machine Check")
DEFINE_EXCEPTION(simd_floating_point_handler, "SIMD Floating Point Exception")
DEFINE_EXCEPTION(virtualization_exception_handler, "Virtualization Exception")
DEFINE_EXCEPTION(control_protection_handler, "Control Protection Exception")

void isr_test_handler(struct isr_stack_frame frame)
{
	vga_write_string("test isr", 17, 20, TEXT_COLOUR_PROCESS);
}

static volatile int ticks = 0;

void programmable_interrupt_timer_handler(struct isr_stack_frame frame)
{
	ticks++;
	vga_write_hex(ticks, 5, 20, TEXT_COLOUR_PROCESS);
	outb(0x20, 0x20);
}

#define DATA_PORT_PS2 0x60
#define STATUS_PORT_PS2 0x64

void keyboard_irq_handler(struct isr_stack_frame frame)
{
	unsigned char scancode = inb(DATA_PORT_PS2);
	//system_flag_key(scancode);
	outb(0x20, 0x20);
}
