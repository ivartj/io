#ifndef IO_H
#define IO_H

#include <stdio.h>

typedef struct io_buf io_buf;
struct io_buf {
	size_t len;
	size_t cap;
	void *buf;
};

void *io_readall(int fd, unsigned *rlen);

size_t io_bufwrite(void *data, size_t size, size_t nmemb, io_buf *buf);

#endif
