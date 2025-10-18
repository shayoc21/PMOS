
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
void scheduler_signal_error(const char* a) {}
void system_panic(const char* a) {}

#define DEFINE_HANDLER(_name,_msg) 							\
void _name(struct isr_stack_frame frame)						\
{											\
	if ((frame.cs & 3) == 3) 							\
	{										\
		scheduler_signal_error(_msg);						\
	} 										\
	else										\
       	{										\
		system_panic(_msg);							\
	}										\
}											
DEFINE_HANDLER(debug_exception_handler, "Debug Exception")
DEFINE_HANDLER(nmi_handler, "Non-Maskable Interrupt")
DEFINE_HANDLER(breakpoint_handler, "Breakpoint")
DEFINE_HANDLER(overflow_handler, "Overflow")
DEFINE_HANDLER(bound_handler, "BOUND Range Exceeded")
DEFINE_HANDLER(invalid_opcode_handler, "Invalid Opcode")
DEFINE_HANDLER(device_not_available_handler, "Device Not Available")
DEFINE_HANDLER(double_fault_handler, "Double Fault")
DEFINE_HANDLER(coprocessor_handler, "Coprocessor Segment Overrun")
DEFINE_HANDLER(invalid_tss_handler, "Invalid TSS")
DEFINE_HANDLER(segment_not_present_handler, "Segment Not Present")
DEFINE_HANDLER(stack_segment_fault_handler, "Stack Segment Fault")
DEFINE_HANDLER(general_protection_fault_handler, "General Protection Fault")
DEFINE_HANDLER(page_fault_handler, "Page Fault")
DEFINE_HANDLER(floating_point_handler, "x87 Floating Point Exception")
DEFINE_HANDLER(alignment_check_handler, "Alignment Check")
DEFINE_HANDLER(machine_check_handler, "Machine Check")
DEFINE_HANDLER(simd_floating_point_handler, "SIMD Floating Point Exception")
DEFINE_HANDLER(virtualization_exception_handler, "Virtualization Exception")
DEFINE_HANDLER(control_protection_handler, "Control Protection Exception")

void isr_test_handler(struct isr_stack_frame frame)
{
	vga_write_string("test isr", 17, 30, TEXT_COLOUR_PROCESS);
}	


