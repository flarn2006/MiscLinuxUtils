#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/stat.h>

int movefile(const char *src, const char *dest);

int main(int argc, char *argv[])
{
	if (argc < 3) {
		fprintf(stderr, "Usage: %s file cmd [args]...\n", argv[0]);
		return 2;
	}

	int pipefd[2];
	pipe2(pipefd, O_CLOEXEC);

	char filename[18] = "apply_XXXXXX";
	int tmpfd = mkstemp(filename);

	if (tmpfd < 0) {
		strcpy(filename, "/tmp/apply_XXXXXX");
		tmpfd = mkstemp(filename);
	}
	
	if (tmpfd < 0) {
		fprintf(stderr, "%s: error creating temporary file: %s\n", argv[0], strerror(errno));
		return 1;
	}
	
	pid_t pid = fork();
	if (pid < 0) {
		fprintf(stderr, "%s: fork: %s\n", argv[0], strerror(errno));
		close(tmpfd);
		unlink(filename);
		return 1;
	} else if (pid > 0) {
		/* parent process */
		close(pipefd[1]);
		close(tmpfd);

		char readbuf[64];
		int success = 1;
		int count;
		while (count = read(pipefd[0], readbuf, 64)) {
			success = 0;
			write(2, readbuf, count);
		}
		if (success) {
			int retval;
			wait(&retval);
			
			if (retval == 0) {
				int movefile_result = movefile(filename, argv[1]);
				if (movefile_result < 0) {
					const char *guilty_filename = (movefile_result == -1) ? filename : argv[1];
					fprintf(stderr, "%s: %s: %s\n", argv[0], guilty_filename, strerror(errno));
					unlink(filename);
					return 1;
				}
			} else {
				unlink(filename);
			}

			return retval;
		} else {
			close(tmpfd);
			unlink(filename);
			return 1;
		}
	} else {
		/* child process */
		close(pipefd[0]);
		
		int infd = open(argv[1], O_RDONLY);
		if (infd < 0) {
			dprintf(pipefd[1], "%s: %s: %s\n", argv[0], argv[1], strerror(errno));
			return 1;
		}
		
		dup2(infd, 0); close(infd);
		dup2(tmpfd, 1); close(tmpfd);
		
		execvp(argv[2], argv+2);
		dprintf(pipefd[1], "%s: %s: %s\n", argv[0], argv[2], strerror(errno));
		close(pipefd[1]);
		return 1;
	}
}

int movefile(const char *src, const char *dest)
{
	int copydata = 0;
	int *linktgt = NULL;

	struct stat st;
	if (lstat(dest, &st) != -1) {
		if (st.st_nlink > 1 || S_ISLNK(st.st_mode))
			copydata = 1;

	} else if (errno != ENOENT) {
		return -1;
	}

	if (!copydata)
		if (rename(src, dest) == -1)
			copydata = 1;

	if (copydata) {
		int result = 0;
		FILE *fpsrc = fopen(src, "r");
		if (!fpsrc) {
			result = -1;
			goto err_open_fpsrc; /* What? Stack Overflow told me it was okay :p */
		}
		FILE *fpdest = fopen(dest, "w");
		if (!fpdest) {
			result = -2;
			goto err_open_fpdest;
		}
		while (1) {
			int ch = fgetc(fpsrc);
			if (ch == EOF) {
				if (feof(fpsrc)) {
					break;
				} else {
					result = -1;
					goto err_copy;
				}
			} else {
				if (fputc(ch, fpdest) == EOF) {
					result = -2;
					goto err_copy;
				}
			}
		}
		unlink(src);
err_copy:
		fclose(fpdest);
err_open_fpdest:
		fclose(fpsrc);
err_open_fpsrc:
		return result;
	}

	return 0;
}
