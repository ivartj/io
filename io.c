#include "io.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void *io_readall(io_reader *r, ssize_t *rlen)
{
	char *buf;
	ssize_t cap, len, inc;

	len = 0;
	cap = 256;
	buf = calloc(cap, 1);
	while((inc = r->read(buf + len, 1, cap - len, r->data)) != 0) {
		len += inc;
		if(len == cap) {
			cap *= 2;
			buf = realloc(buf, cap);
			memset(buf + len, 0, cap - len);
		} else
			break;
	}

	if(rlen != NULL)
		*rlen = len;

	return buf;
}

void *io_fdreadall(int fd, unsigned *rlen)
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

	if(rlen != NULL)
		*rlen = len;

	return data;
}

size_t io_bufread(void *data, size_t size, size_t nitems, void *vbuf)
{
	size_t scribed;
	io_buf *buf;

	buf = (io_buf *)vbuf;

	if(buf->len == 0)
		return 0;

	scribed = nitems * size;
	if(scribed > buf->len - buf->off) {
		scribed = buf->len - buf->off;
		nitems = scribed / size;
		scribed = nitems * size; 
	}

	memcpy(data, buf->buf + buf->off, scribed);
	buf->off += scribed;

	return nitems;
}

size_t io_bufwrite(void *data, size_t size, size_t nmemb, void *vbuf)
{
	size_t written;
	size_t reqsize;
	int changed;
	io_buf *buf;

	buf = (io_buf *)vbuf;

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

	size = io_vasprintf(&buf, format, ap);
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
	va_end(ap);
}

int io_putc(io_writer *io, unsigned char c)
{
	return io->write(&c, 1, 1, io->data);
}

int io_getc(io_reader *r)
{
	size_t n;
	unsigned char c;

	n = r->read(&c, 1, 1, r->data);
	if(n == 0)
		return EOF;
	return c;
}

int io_vasprintf(char **ptr, const char *format, va_list oap)
{
	char *str;
	size_t size;
	va_list ap;

	va_copy(ap, oap);
	size = vsnprintf(NULL, 0, format, ap);
	va_end(ap);

	str = malloc(size + 1);
	*ptr = str;

	return vsnprintf(str, size + 1, format, oap);
}

ssize_t io_getdelim(char **linep, size_t *linecapp, int delim, io_reader *r)
{
	int c;
	ssize_t len;

	len = 0;

	for(;;) {
		c = io_getc(r);
		if(c == EOF)
			break;

		if(*linecapp == 0 || *linep == NULL) {
			*linecapp = 256;
			*linep = calloc(*linecapp, sizeof(char));
		}

		if(len + 1 == *linecapp) {
			*linecapp *= 2;
			*linep = realloc(*linep, *linecapp * sizeof(char));
			memset(*linep + *linecapp / 2, 0, *linecapp / 2);
		}

		(*linep)[len++] = c;

		if(c == '\n')
			break;
	}

	if(c != EOF)
		(*linep)[len] = '\0';
	else
		return -1;

	return len;
}

ssize_t io_getline(char **linep, size_t *linecapp, io_reader *r)
{
	return io_getdelim(linep, linecapp, '\n', r);
}

size_t io_fread(void *ptr, size_t size, size_t nitems, void *stream)
{
	return fread(ptr, size, nitems, stream);
}

size_t io_fwrite(void *ptr, size_t size, size_t nitems, void *stream)
{
	return fwrite(ptr, size, nitems, stream);
}

void io_route(io_reader *r, io_writer *w)
{
	int cap = 1024;
	char buf[1024];
	int len;

	do {
		len = r->read(buf, 1, cap, r->data);
		if(len == 0)
			return;
		w->write(buf, 1, len, w->data);
	} while(len == cap);
}
