#include "../io.h"
#include <stdlib.h>
#include <assert.h>

int main(int argc, char *argv[])
{
	io_buf buf;
	io_writer w;

	memset(&buf, 0, sizeof(buf));
	w.write = io_bufwrite;
	w.data = &buf;

	io_printf(&w, "%d", 123);
	puts(buf.buf);
	assert(strcmp(buf.buf, "123") == 0);

	exit(EXIT_SUCCESS);
}
