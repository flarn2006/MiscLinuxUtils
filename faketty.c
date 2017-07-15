#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>

int main(int argc, char *argv[])
{
	if (argc != 2) {
		printf("Usage: %s tty\n", argv[0]);
		return 2;
	}

	int fd = open(argv[1], O_WRONLY | O_NOCTTY);
	if (fd == -1) {
		perror(argv[1]);
		return 1;
	}

	int rawmode = 0;
	struct termios tios_saved;
	if (isatty(0)) {
		rawmode = 1;
		puts("Terminal now in raw mode; input will be sent as typed without pressing ENTER.");
		puts("Press C-a C-d to exit, or C-a C-a to send C-a.");
		struct termios tios;
		tcgetattr(0, &tios);
		memcpy(&tios_saved, &tios, sizeof(struct termios));
		cfmakeraw(&tios);
		tcsetattr(0, TCSANOW, &tios);
	}

	int error = 0;
	int ctrla = 0;
	int c; while ((c = getchar()) != EOF) {
		if (!ctrla && c == 1) {
			ctrla = 1;
		} else if (ctrla && c == 4) {
			break;
		} else {
			ctrla = 0;
			c &= 0xFF;
			if (ioctl(fd, TIOCSTI, &c) == -1) {
				putchar('\n');
				perror(argv[1]);
				error = 1;
				break;
			}
		}
	}
	
	if (rawmode) {
		tcsetattr(0, TCSANOW, &tios_saved);
	}

	close(fd);
	return error;
}
