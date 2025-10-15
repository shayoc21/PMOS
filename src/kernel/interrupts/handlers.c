
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
struct ring0_isr_stack_frame
{
	unsigned int gs, fs, es, ds;
	unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
	unsigned int err_code;
	unsigned int handler_address;	//can be ignored
	unsigned int eip;
	unsigned int cs;
	unsigned int eflags;
};
struct ring3_isr_stack_frame
{
	struct ring0_isr_stack_frame;
	unsigned int user_esp, user_ss;
};

//exception handler functions
void divide_by_zero_handler           (void) {}
void debug_exception_handler          (void) {}
void nmi_handler                      (void) {}
void breakpoint_handler               (void) {}
void overflow_handler                 (void) {}
void bound_handler                    (void) {}
void invalid_opcode_handler           (void) {}
void device_not_available_handler     (void) {}
void double_fault_handler             (void) {}
void coprocessor_handler              (void) {}
void invalid_tss_handler              (void) {}
void segment_not_present_handler      (void) {}
void stack_segment_fault_handler      (void) {}
void general_protection_fault_handler (void) {}
void page_fault_handler               (void) {}
void floating_point_handler           (void) {}
void alignment_check_handler          (void) {}
void machine_check_handler            (void) {}
void simd_floating_point_handler      (void) {}
void virtualization_exception_handler (void) {}
void control_protection_handler       (void) {}

