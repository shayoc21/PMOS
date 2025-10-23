#pragma once
#include <stdarg.h>

struct M_Text_Buffer
{
    char* buffer;
    size_t buffer_size;
    int buffer_cursor_pos;
    int buffer_max_length;
};

typedef enum M_STREAM_PRIVILEGE
{
    USER, KERNEL
} M_STREAM_PRIVILEGE;

struct M_Stream
{
    struct M_Text_Buffer text;
    M_STREAM_PRIVILEGE privilege; //privileged streams will only be readable from kernel-space programs
};
static struct M_Stream m_stdout, m_stderr, kernel_debug;
//assumes buffer is an allocated buffer
void create_stream(const struct M_Stream* stream, struct M_Text_Buffer buffer, M_STREAM_PRIVILEGE privilege);

//printf will print to m_stdout, eprintf to m_stderr.. prints a formatted string to the stream, updating the streams cursor position
void printf(const char* format, ...);
void eprintf(const char* format, ...);
//fprintf lets the user select a stream to print to
void fprintf(struct M_Stream stream, const char* format, ...);
//sprintf prints to a buffer
void sprintf(const void* buffer, const char* format, ...);