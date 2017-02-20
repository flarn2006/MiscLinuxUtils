#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

int main(int argc, char *argv[])
{
	int relative = 0;
	int fd = 0;
	int nextarg = 1;

	if (argc < 2) {
	arg_error:
		fprintf(stderr, "Usage: %s [-r] [fd] offset\n", argv[0]);
		return 2;
	}

	if (!strcmp(argv[1], "-r")) {
		relative = 1;
		nextarg = 2;
	}

	int remaining_args = argc - nextarg;
	if (remaining_args < 1 || remaining_args > 2)
		goto arg_error;

	int scanf_result;

	if (remaining_args == 2) {
		scanf_result = sscanf(argv[nextarg], "%d", &fd);
		if (scanf_result == 0 || scanf_result == EOF)
			goto arg_error;
	}

	intmax_t intmax;
	scanf_result = sscanf(argv[argc-1], "%jd", &intmax);
	if (scanf_result == 0 || scanf_result == EOF)
		goto arg_error;
	
	int whence;
	if (relative)
		whence = SEEK_CUR;
	else
		whence = (argv[argc-1][0] == '-') ? SEEK_END : SEEK_SET;
		
	if (lseek(fd, (off_t)intmax, whence) == (off_t)-1) {
		perror(argv[0]);
		return 1;
	}

	return 0;
}
