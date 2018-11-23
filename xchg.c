#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <linux/fs.h>
#include <syscall.h>

int main(int argc, char *argv[])
{
	const char *error_src = NULL;
	
	if (argc != 3) {
		fprintf(stderr, "Usage: %s FILE1 FILE2\n", argv[0]);
		return 255;
	}

	if (access(argv[2], F_OK) == -1)
		error_src = argv[2];
	else if (syscall(SYS_renameat2, AT_FDCWD, argv[1], AT_FDCWD, argv[2], RENAME_EXCHANGE) == -1)
		if (errno == ENOENT) {
			error_src = argv[1];
		} else if (errno == EISDIR || errno == ENOTDIR) {
			error_src = argv[2];
		} else {
			error_src = "renameat2";
		}

	if (error_src) {
		fprintf(stderr, "%s: %s: %s\n", argv[0], error_src, strerror(errno));
		return 1;
	} else {
		return 0;
	}
}
