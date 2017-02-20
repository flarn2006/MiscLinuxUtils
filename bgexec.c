#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr, "Usage: %s command [arg]...\n", argv[0]);
		return 2;
	}
	
	int errtty = isatty(2);

	int pipefd[2];
	pipe2(pipefd, O_CLOEXEC);
	pid_t pid = fork();

	if (pid < 0) {
		fprintf(stderr, "%s: fork: %s\n", argv[0], strerror(errno));
		return 1;
	} else if (pid == 0) {
		/* child process */
		close(pipefd[0]);

		int errfd;
		if (errtty)
			errfd = fcntl(2, F_DUPFD_CLOEXEC);
		else
			errfd = 2;

		int nullfd = open("/dev/null", O_RDWR | O_CLOEXEC);
		if (nullfd < 0) {
			/* error opening /dev/null */
			/* shouldn't happen, but just close the FD's and continue anyway */
			dprintf(errfd, "%s: couldn't open /dev/null: %s\n", argv[0], strerror(errno));
			if (isatty(0)) close(0);
			if (isatty(1)) close(1);
			if (errtty) close(2);
		} else {
			if (isatty(0)) dup2(nullfd, 0);
			if (isatty(1)) dup2(nullfd, 1);
			if (errtty) dup2(nullfd, 2);
		}
		execvp(argv[1], argv+1);
		
		/* if execvp returns, there was an error */
		dprintf(errfd, "%s: %s: %s\n", argv[0], argv[1], strerror(errno));
		write(pipefd[1], "", 1); /* notify parent process of failure */
		close(pipefd[1]);
		return 1;
	} else {
		/* parent process */
		close(pipefd[1]);
		if (read(pipefd[0], &pipefd[1], 1) > 0) {
			return 1;
		} else {
			/*if (errtty)
				fprintf(stderr, "process %u created\n", pid);*/
			return 0;
		}
	}
}
