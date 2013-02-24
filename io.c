#include "io.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void *io_readall(int fd, unsigned *rlen)
{
	unsigned cap, len;
	int inc;
	void *data;

	cap = 256;
	len = 0;
	data = malloc(cap);

	while((inc = read(fd, data + len, cap - len)) > 0) {
		len += inc;
		cap *= 2;
		data = realloc(data, cap);
	}

	if(len < 0) {
		free(data);
		return NULL;
	}

	if(rlen != NULL)
		*rlen = len;

	return data;
}

size_t io_bufwrite(void *data, size_t size, size_t nmemb, io_buf *buf)
{
	size_t written;
	size_t reqsize;
	int changed;

	changed = 0;
	reqsize = buf->len + size * nmemb;
	while(buf->cap < reqsize) {
		if(buf->cap == 0)
			buf->cap = 256;
		else
			buf->cap <<= 1;
		changed = 1;
	}
	if(changed)
		buf->buf = realloc(buf->buf, buf->cap);

	written = size * nmemb;
	memcpy(buf->buf + buf->len, data, written);
	buf->len += written;

	return nmemb;
}

int io_vprintf(io_writer *io, const char *format, va_list ap)
{
	char *buf;
	int size;
	size_t retval;

	size = vasprintf(&buf, format, ap);
	if(retval < 0)
		return -1;

	retval = io->write(buf, 1, size, io->data);
	free(buf);
	if(retval != size)
		return -1;

	return retval;
}

int io_printf(io_writer *io, const char *format, ...)
{
	va_list ap;

	va_start(ap, format);
	return io_vprintf(io, format, ap);
}
