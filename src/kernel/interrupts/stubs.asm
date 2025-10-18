bits 32

%macro SAVE_ALL 0
	pusha
	push ds
	push es
	push fs
	push gs
%endmacro
%macro RESTORE_ALL 0
	pop gs
	pop fs
	pop es
	pop ds
	popa
%endmacro	
%macro LOAD_DATA_SEGMENT 1
	mov ax, %1
	mov ds, ax
	mov es, ax
	mov fs, word ax
	mov gs, word ax
%endmacro

%define KERNEL_DATA_SEGMENT 0x10

isr_common:
	SAVE_ALL
	LOAD_DATA_SEGMENT(KERNEL_DATA_SEGMENT)
	
	call [esp + 48]
	
	RESTORE_ALL
	add esp, 8
	iret

%macro ALIGN 1
	align %1, db 0x90
%endmacro
%macro ENTRY_ERROR 2
	extern %2
	ALIGN 4
	global %1
	%1:
		push dword %2  ;%2 will be the label of the c handler
		jmp isr_common
		
%endmacro
%macro ENTRY_NO_ERROR 2
	extern %2
	ALIGN 4
	global %1
	%1:
		push dword 0   ;no error, still have to push an error code (arbitrary)
		push dword %2
		jmp isr_common
%endmacro

ENTRY_NO_ERROR	divide_by_zero, divide_by_zero_handler
ENTRY_NO_ERROR	debug_exception, debug_exception_handler
ENTRY_NO_ERROR	non_maskable_interrupt, nmi_handler
ENTRY_NO_ERROR	breakpoint, breakpoint_handler
ENTRY_NO_ERROR	overflow, overflow_handler
ENTRY_NO_ERROR	bound_range_exceeded, bound_handler
ENTRY_NO_ERROR	invalid_opcode, invalid_opcode_handler
ENTRY_NO_ERROR	device_not_available, device_not_available_handler
ENTRY_NO_ERROR	double_fault, double_fault_handler
ENTRY_NO_ERROR	coprocessor_segment_overrun, coprocessor_handler
ENTRY_ERROR	invalid_tss, invalid_tss_handler
ENTRY_ERROR	segment_not_present, segment_not_present_handler
ENTRY_ERROR	stack_segment_fault, stack_segment_fault_handler
ENTRY_ERROR	general_protection_fault, general_protection_fault_handler
ENTRY_ERROR	page_fault, page_fault_handler
ENTRY_NO_ERROR	floating_point_error, floating_point_handler
ENTRY_ERROR	alignment_check, alignment_check_handler
ENTRY_NO_ERROR	machine_check, machine_check_handler
ENTRY_NO_ERROR	simd_floating_point, simd_floating_point_handler
ENTRY_NO_ERROR	virtualization_exception, virtualization_exception_handler
ENTRY_NO_ERROR	control_protection, control_protection_handler

ENTRY_NO_ERROR isr_test, isr_test_handler


