OUTPUT_FORMAT("binary")
ENTRY(kernel_entry)
SECTIONS
{
	. = 0xC0000000;
	.stack 0xC0000000 :
	{
		KEEP(*(.stack))
	}
	.text :
	{
		KEEP(*(.text.kernel_entry))
		*(.text)
	}
	.data :
	{
		*(.data)
	}
	.bss :
	{
		*(.bss)
		*(COMMON)
	}
}
