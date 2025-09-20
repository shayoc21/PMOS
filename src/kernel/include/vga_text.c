#pragma once

#define TEXT_COLOUR_PROCESS 0x0E
#define TEXT_COLOUR_FAILURE 0x04
#define TEXT_COLOUR_SUCCESS 0x0A
#define TEXT_COLOUR_USER

#define VGA_TEXT_BUFFER (volatile char*)0xB8000
#define VGA_TEXT_LINE_WIDTH 80
#define VGA_TEXT_COLUMN_HEIGHT 25
#define VGA_TEXT_CHARACTER_SIZE 0x02

#include "./hardware_io.c"

void vga_write_string(const char* string, int line, int character, unsigned char text_colour_background)
{
	volatile char* ptr = VGA_TEXT_BUFFER + (line*VGA_TEXT_LINE_WIDTH + character)*VGA_TEXT_CHARACTER_SIZE;
	while (*string)
	{
		*ptr++ = *string++;
		*ptr++ = text_colour_background;
	}
	return;
}
//prints 4 bytes of hex in big endian
void vga_write_hex(const int hex, int line, int character, unsigned char text_colour_background)
{
	char buf[9];
	const char* digits = "0123456789ABCDEF";
	buf[0] = '0';
	buf[1] = 'x';
	buf[2] = digits[hex>>28 & 0x0F];
	buf[3] = digits[hex>>24 & 0x0F];
	buf[4] = digits[hex>>20 & 0x0F];
	buf[5] = digits[hex>>16 & 0x0F];
	buf[6] = digits[hex>>12 & 0x0F];
	buf[7] = digits[hex>> 8 & 0x0F];
	buf[8] = digits[hex>> 4 & 0x0F];
	buf[9] = digits[hex>> 0 & 0x0F];
	vga_write_string(buf, line, character, text_colour_background);
	return;
}

void vga_clear()
{
	const int buffer_size = VGA_TEXT_LINE_WIDTH * VGA_TEXT_COLUMN_HEIGHT * VGA_TEXT_CHARACTER_SIZE;
	for (int ptr = 0; ptr < buffer_size;) { *(unsigned char*)ptr++ = 0x00; *(unsigned char*)ptr++ = 0x00; };
	return;
}

void vga_disable_cursor()
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
}

void vga_enable_cursor(unsigned char cursor_start, unsigned char cursor_end)
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);
	outb(0x3D5, 0x0B);
	outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
}

void vga_move_cursor(int x, int y)
{
	unsigned short pos = y * VGA_TEXT_LINE_WIDTH + x;
	outb(0x3D4, 0x0F);
	outb(0x3D5, (unsigned char) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (unsigned char) ((pos >> 8) & 0xFF));
}


