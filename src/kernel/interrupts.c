// I hate the code in this file so much, it needs a rewrite at some point
// I could probably use macros to make the extern definitions less horrible to look at

extern void isr_stub_no_err_0(void);
extern void isr_stub_no_err_1(void);
extern void isr_stub_no_err_2(void);
extern void isr_stub_no_err_3(void);
extern void isr_stub_no_err_4(void);
extern void isr_stub_no_err_5(void);
extern void isr_stub_no_err_6(void);
extern void isr_stub_no_err_7(void);
extern void isr_stub_no_err_9(void);
extern void isr_stub_no_err_15(void);
extern void isr_stub_no_err_16(void);
extern void isr_stub_no_err_17(void);
extern void isr_stub_no_err_18(void);
extern void isr_stub_no_err_19(void);
extern void isr_stub_no_err_20(void);

extern void isr_stub_err_8(void);
extern void isr_stub_err_10(void);
extern void isr_stub_err_11(void);
extern void isr_stub_err_12(void);
extern void isr_stub_err_13(void);
extern void isr_stub_err_14(void);
extern void isr_stub_err_21(void);


struct idt_entry
{
	u16_t offset_low;
	u16_t segment_selector;
	u8_t reserved;
	u8_t flags;
	u16_t offset_high;
} __attribute__((packed));
static struct idt_entry idt[256];
struct interrupt
{
	void(*isr_ptr)(void);
	u8_t flags;
};
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
}

void initialise_interrupt( unsigned int vector, void (*isr)(void), unsigned short segment, unsigned char flags )
{
	idt[vector] = 
	{
		.offset_low = (unsigned short)((unsigned int)isr & 0xFFFF);
		.segment_selector = segment;
		.reserved = 0x00;
		.flags = flags;
		.offset_high = (unsigned short)(((unsigned int)isr >> 16) & 0xFFFF);
	};
}

#define KERNEL_DATA 0x10

void initialise_interrupt_descriptor_table()
{
	// Exceptions without error code
	initialise_interrupt(0,  isr_stub_no_err_0,  KERNEL_DATA, 0b10001111);
	initialise_interrupt(1,  isr_stub_no_err_1,  KERNEL_DATA, 0b10001111);
	initialise_interrupt(3,  isr_stub_no_err_3,  KERNEL_DATA, 0b10001110);
	initialise_interrupt(4,  isr_stub_no_err_4,  KERNEL_DATA, 0b10001111);
	initialise_interrupt(5,  isr_stub_no_err_5,  KERNEL_DATA, 0b10001111);
	initialise_interrupt(6,  isr_stub_no_err_6,  KERNEL_DATA, 0b10001111);
	initialise_interrupt(7,  isr_stub_no_err_7,  KERNEL_DATA, 0b10001111);
	initialise_interrupt(9,  isr_stub_no_err_9,  KERNEL_DATA, 0b10001111);
	initialise_interrupt(15, isr_stub_no_err_15, KERNEL_DATA, 0b10001111); 

	// Exceptions with error code
	initialise_interrupt(2,  isr_stub_err_2,  KERNEL_DATA, 0b10001111); 
	initialise_interrupt(8,  isr_stub_err_8,  KERNEL_DATA, 0b10001111); 
	initialise_interrupt(10, isr_stub_err_10, KERNEL_DATA, 0b10001111); 
	initialise_interrupt(11, isr_stub_err_11, KERNEL_DATA, 0b10001111); 
	initialise_interrupt(12, isr_stub_err_12, KERNEL_DATA, 0b10001111); 
	initialise_interrupt(13, isr_stub_err_13, KERNEL_DATA, 0b10001111); 
	initialise_interrupt(14, isr_stub_err_14, KERNEL_DATA, 0b10001111); 
	initialise_interrupt(16, isr_stub_err_16, KERNEL_DATA, 0b10001111);
	initialise_interrupt(17, isr_stub_err_17, KERNEL_DATA, 0b10001111);
	initialise_interrupt(18, isr_stub_err_18, KERNEL_DATA, 0b10001111);
	initialise_interrupt(19, isr_stub_err_19, KERNEL_DATA, 0b10001111);
	initialise_interrupt(20, isr_stub_err_20, KERNEL_DATA, 0b10001111); 
	// Reserved: 21-31
	initialise_interrupt(21, isr_stub_no_err_21, KERNEL_DATA, 0b10001111);
	initialise_interrupt(22, isr_stub_no_err_22, KERNEL_DATA, 0b10001111);
	initialise_interrupt(23, isr_stub_no_err_23, KERNEL_DATA, 0b10001111);
	initialise_interrupt(24, isr_stub_no_err_24, KERNEL_DATA, 0b10001111);
	initialise_interrupt(25, isr_stub_no_err_25, KERNEL_DATA, 0b10001111);
	initialise_interrupt(26, isr_stub_no_err_26, KERNEL_DATA, 0b10001111);
	initialise_interrupt(27, isr_stub_no_err_27, KERNEL_DATA, 0b10001111);
	initialise_interrupt(28, isr_stub_no_err_28, KERNEL_DATA, 0b10001111);
	initialise_interrupt(29, isr_stub_no_err_29, KERNEL_DATA, 0b10001111);
	initialise_interrupt(30, isr_stub_no_err_30, KERNEL_DATA, 0b10001111);
	initialise_interrupt(31, isr_stub_no_err_31, KERNEL_DATA, 0b10001111);	

struct interrupt_stack_frame
{
	unsigned int gs, fs, es, ds;
	unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
	unsigned int int_no, err_code;
	unsigned int eip, cs, eflags, useresp, ss;
};

static const char** exception_messages =
{
	"Division By Zero",                
	"Debug",                           
	"Non Maskable Interrupt",          
	"Breakpoint",                      
	"Overflow",                        
	"Bound Range Exceeded",            
	"Invalid Opcode",                  
	"Device Not Available (No Math Coprocessor)", 
	"Double Fault",                    
	"Coprocessor Segment Overrun (reserved)", 
	"Invalid TSS",                     
	"Segment Not Present",             
	"Stack-Segment Fault",             
	"General Protection Fault",        
	"Page Fault",                      
	"Reserved",                        
	"x87 Floating-Point Exception",    
	"Alignment Check",                 
	"Machine Check",                   
	"SIMD Floating-Point Exception",   
	"Virtualization Exception",        
	"Control Protection Exception"    // (Intel CET)
};

//cpu exceptions
void cpu_exception_handler(struct interrupt_stack_frame* frame)
{
	if (frame->int_no <= 21) { vga_write_string(exception_messages[frame->int_no], 24, 0, TEXT_COLOUR_FAILURE); }
	else { vga_write_string("Unhandled Exception", 24, 0, TEXT_COLOUR_FAILURE); }
	vga_write_string("Halting..", 24, 60, TEXT_COLOUR_FAILURE);
	for (;;);
}

static (*isr_handler_table[])(struct interrupt_stack_frame*) = 
{
	cpu_exception_handler
};

void isr_fault_handler(struct interrupt_stack_frame* frame)
{
	if (frame->int_no < 32) { isr_handler_table[0](frame); }
	else { isr_handler_table[frame->int_no](frame); }
	return;
}





