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

void vga_write_string(const char* string, int line, int character, unsigned char text_colour_background);
void vga_write_hex(const int hex, int line, int character, unsigned char text_colour_background);
void vga_clear();
void vga_disable_cursor();
void vga_enable_cursor(unsigned char cursor_start, unsigned char cursor_end);
void vga_move_cursor(int x, int y);



