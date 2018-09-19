#define _GNU_SOURCE
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sched.h>
#include <errno.h>
#include <string.h>

int main(int argc, char *argv[])
{
	char * const argv_default[] = {"bash", NULL};
	char * const *exec_argv = argv_default;

	if (argc > 2) {
		exec_argv = argv+2;
	} else if (argc < 2) {
		fprintf(stderr, "Usage: %s ns-path [command [args...]]\n", argv[0]);
		return 255;
	}

	int fd = open(argv[1], O_RDONLY);
	if (fd == -1) {
		fprintf(stderr, "%s: %s: %s\n", argv[0], argv[1], strerror(errno));
		return 254;
	}

	if (setns(fd, 0) == -1) {
		perror(argv[0]);
		close(fd);
		return 253;
	}

	close(fd);
	execvp(exec_argv[0], exec_argv);
	/* if exec fails... */
	fprintf(stderr, "%s: %s: %s\n", argv[0], exec_argv[0], strerror(errno));
	return 252;
}
