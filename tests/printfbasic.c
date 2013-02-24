#include "../io.h"
#include <stdlib.h>
#include <assert.h>

int main(int argc, char *argv[])
{
	io_buf buf;
	io_writer writer;

	memset(&buf, 0, sizeof(buf));
	memset(&writer, 0, sizeof(writer));
	writer.write = io_bufwrite;
	writer.data = &buf;

	io_printf(&writer, "test");

	assert(strcmp(buf.buf, "test") == 0);

	exit(EXIT_SUCCESS);
}
