OUTPUT_FORMAT("elf32-i386")
ENTRY(loader_entry)
SECTIONS
{
	. = 0x8200;
	.text :
	{
		KEEP(*(.text.loader_entry))
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
