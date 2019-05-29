#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr, "Usage: %s CMD [ARGS...]\n", argv[0]);
		return 255;
	}

	if (fork()) {
		/* Parent process */
		int retval;
		wait(&retval);
		return !retval;
	} else {
		/* Child process */
		execvp(argv[1], argv+1);
		perror(argv[1]);
		return 255;
	}
}
