%macro isr_stub_no_err 1	; Macro for ISRs with either no error or a zero error (Double Fault, Alignment Check)
isr_stub_no_err_%1:
	cli
	push byte 0
	push byte %1
	jmp isr_common
%endmacro
%macro isr_stub_err 1
isr_stub_err_%1:
	cli
	push byte %1
	jmp isr_common
%endmacro

;explicit definition of the x86 hardware interrupts
isr_stub_no_err 0
isr_stub_no_err 1
isr_stub_no_err 2
isr_stub_no_err 3
isr_stub_no_err 4
isr_stub_no_err 5
isr_stub_no_err 6
isr_stub_no_err 7
isr_stub_err    8
isr_stub_no_err 9
isr_stub_err    10
isr_stub_err    11
isr_stub_err    12
isr_stub_err    13
isr_stub_err    14
isr_stub_no_err 15
isr_stub_no_err 16
isr_stub_no_err 17
isr_stub_no_err 18
isr_stub_no_err 19
isr_stub_no_err 20
isr_stub_err    21

;no_err software interrupt macro
%assign i 32
%rep 224
	isr_stub_no_err i
	%assign i i+1
%endrep

extern isr_fault_handler

isr_common:
	pusha
	push ds
	push es
	push fs
	push gs
	mov ax, 0x10 ;kernel data segment
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov eax, esp
	push eax
	mov eax, isr_fault_handler
	call eax
	pop eax
	pop gs
	pop fs
	pop es
	pop ds
	popa
	add esp, 8
	iret
