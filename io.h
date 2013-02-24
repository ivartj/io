#ifndef IO_H
#define IO_H

#include <stdio.h>
#include <stdarg.h>

typedef struct io_buf io_buf;
typedef struct io_writer io_writer;

struct io_buf {
	size_t len;
	size_t cap;
	void *buf;
};

struct io_writer {
	void *data;
	size_t (*write)(void *ptr, size_t size, size_t nitems, void *data);
};

size_t io_vprintf(io_writer *io, const char *format, va_list ap);
size_t io_printf(io_writer *io, const char *format, ...);

void *io_readall(int fd, unsigned *rlen);

size_t io_bufwrite(void *data, size_t size, size_t nmemb, io_buf *buf);

#endif
