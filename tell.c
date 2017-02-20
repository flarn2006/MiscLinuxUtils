#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	if (argc > 2) {
		fprintf(stderr, "Usage: %s [fd]\n", argv[0]);
		return 2;
	}

	int fd = 0;
	if (argc == 2) {
		if (sscanf(argv[1], "%d", &fd) == 0 || fd < 0) {
			fprintf(stderr, "%s: File descriptor must be an integer >=0\n", argv[0]);
			return 2;
		}
	}

	off_t pos = lseek(fd, 0, SEEK_CUR);

	if (pos == (off_t)-1) {
		perror(argv[0]);
		return 1;
	}

	fprintf(stderr, "%jd\n", (intmax_t)pos);

	return 0;
}
