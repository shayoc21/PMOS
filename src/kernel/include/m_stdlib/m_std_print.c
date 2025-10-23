#include "m_std_print.h"

//users are allowed to create streams. at any time they can print the content of a stream in a terminal.
//i do not like automatic printing. i would like to probe the program for output, it reduces clutter.
//I will create a terminal command "stream print -n <stream_name>". Programs can share streams.
//users are allowed to make kernel streams, they cannot edit/view existing kernel streams
void create_stream(struct M_Stream* stream, struct M_Text_Buffer buffer, M_STREAM_PRIVILEGE privilege)
{
    //buffer already allocated
    stream->buffer = buffer;
    stream->privilege = privilege;
    return;
}

void m_buffer_write_string(M_Text_Buffer* buffer, const char* string)
{
    for (int c = 0;buffer.buffer_cursor_pos < buffer.buffer_max_length;)
    {
        if (string[c] == 0) return; // null terminator
        buffer.buffer[buffer.buffer_cursor_pos++] = string[c++];
    }
    return;
}

//this function was shamefully borrowed from chatgpt
void int_to_str(int value, char *buffer) {
	char temp[12];	// enough for -2^31 and null terminator
	int i = 0, j = 0;
	int is_negative = 0;

	if (value == 0) {
		buffer[0] = '0';
		buffer[1] = '\0';
		return;
	}

	if (value < 0) {
		is_negative = 1;
		value = -value;
	}

	// extract digits in reverse order
	while (value > 0) {
		temp[i++] = (value % 10) + '0';
		value /= 10;
	}

	if (is_negative)
		buffer[j++] = '-';

	// reverse into buffer
	while (i > 0)
		buffer[j++] = temp[--i];

	buffer[j] = '\0';
}

void m_buffer_write_int(M_Text_Buffer* buffer, int data)
{
    char buf[12];
    m_int_to_str(data, &buf);
    m_buffer_write_string(buffer, buf);
}

void vfprintf(const struct M_Stream, const char* format, va_list arg)
{
    for (int c = 0; M_Stream.text.buffer_cursor_pos < M_Stream.text.buffer_max_length; c++)
    {
        const char ch = format[c]
        if (ch == 0) return; //null terminator
        if (ch == '%')
        {
            c++;
            switch (format[c])
            {
                case 'd':
                    m_buffer_write_int(&M_Stream.text, va_arg(arg, int));
                    break;
                case 's':
                    const char* st = va_arg(arg, const char*);
                    m_buffer_write_string(&M_Stream.text, st, m_strlen(st));
                    break;
                case 'x':
                    //prints 4 bytes of hexadecimal
                    int hex = va_arg(arg, int);
                    char buf[10];
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
                    m_buffer_write_string(&M_Stream.text, buf);
                    break;
                case 'xt':
                    //truncated hex, prints 2 bytes
                    int hex = va_arg(arg, int);
                    char buf[6];
                    const char* digits = "0123456789ABCDEF";
                    buf[0] = '0';
                    buf[1] = 'x';
                    buf[2] = digits[hex>>12 & 0x0F];
                    buf[3] = digits[hex>> 8 & 0x0F];
                    buf[4] = digits[hex>> 4 & 0x0F];
                    buf[5] = digits[hex>> 0 & 0x0F];
                    m_buffer_write_string(&M_Stream.text, buf);
                    break;
                case 'f':
                    float f = va_arg(arg, float);
                    const char* m = "No float printing yet..";
                    m_buffer_write_string(&M_Stream.text, m, m_strlen(m));
                    break;
            }
        }
        else
        {
            m_buffer_write(&M_Stream.text, ch, sizeof(char));
        }

    }
    //returns by default if the buffer cursor exceeds max length
    return;
}

void printf(const char* format, ...)
{
    va_list arg;
    int done;

    va_start (arg, format);
    done = vfprintf (m_stdout, format, arg);
    va_end (arg);

    return done;
}
void eprintf(const char* format, ...)
{
    va_list arg;
    int done;

    va_start (arg, format);
    done = vfprintf (m_stderr, format, arg);
    va_end (arg);

    return done;
}