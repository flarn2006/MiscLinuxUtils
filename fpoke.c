#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

int hex_digit_value(char digit)
{
	if ('0' <= digit && digit <= '9')
		return digit - '0';
	else if ('A' <= digit && digit <= 'F')
		return digit - 'A' + 0xA;
	else if ('a' <= digit && digit <= 'f')
		return digit - 'a' + 0xa;
	else
		return -1;
}

int main(int argc, char *argv[])
{
	/* Verify command line arguments */
	if (argc < 5) {
		fprintf(stderr, "Usage: %s filename offset type data\n", argv[0]);
		fprintf(stderr, " - type can be: bytes | string\n");
		fprintf(stderr, "ex: %s /proc/1234/mem 8f000014 bytes 01 ff 02\n", argv[0]);
		return 2;
	}

	uintmax_t offset;
	int scanf_result = sscanf(argv[2], "%jx", &offset);
	if (scanf_result == 0 || scanf_result == EOF) {
		fprintf(stderr, "%s: invalid offset '%s'\n", argv[0], argv[2]);
		return 2;
	}

	enum { TYPE_BYTES, TYPE_STRING } datatype;
	if (!strcmp(argv[3], "b") || !strcmp(argv[3], "bytes")) {
		datatype = TYPE_BYTES;
	} else if (!strcmp(argv[3], "s") || !strcmp(argv[3], "str") || !strcmp(argv[3], "string")) {
		datatype = TYPE_STRING;
	} else {
		fprintf(stderr, "%s: unknown type argument '%s'\n", argv[0], argv[3]);
		return 2;
	}
		
	/* Open the target file, and seek to the correct location */
	int fd = open(argv[1], O_WRONLY);
	if (fd == -1) {
		perror(argv[1]);
		return 1;
	}

	if (lseek(fd, (long)offset, SEEK_SET) == -1) {
		close(fd);
		perror(argv[1]);
		return 1;
	}

	if (datatype == TYPE_BYTES) {
		/* Make sure all arguments are valid hex values */
		size_t i; for (i=4; i<argc; ++i) {
			char *ptr = argv[i];
			while (*ptr) {
				if (*ptr != ' ' && hex_digit_value(*ptr) == -1) {
					fprintf(stderr, "%s: invalid hex digit '%c'\n", argv[0], *ptr);
					close(fd);
					return 2;
				}
				++ptr;
			}
		}

		/* Now go through the arguments again and write the data */
		i = 4;
		char *ptr = argv[i];
		while (i < argc) {
			int digit_num = 0;
			char *beginning = ptr;
			while (*ptr && *ptr != ' ') {
				digit_num ^= 1;
				++ptr;
			}
			ptr = beginning;

			uint8_t byte = 0;
			while (*ptr && *ptr != ' ') {
				int value = hex_digit_value(*ptr);
				if (value != -1) {
					if (digit_num == 0) {
						byte = value << 4;
						digit_num = 1;
					} else {
						byte |= value;
						if (write(fd, &byte, 1) == -1) {
							close(fd);
							perror(argv[1]);
							return 1;
						}
						digit_num = 0;
					}
				}
				++ptr;
			}

			if (*ptr == '\0')
				ptr = argv[++i];
			else
				++ptr;
		}
	} else if (datatype == TYPE_STRING) {
		size_t i; for (i=4; i<argc; ++i) {
			int error = 0;
			if (i > 4) {
				char space = ' ';
				error = (write(fd, &space, 1) == -1);
			}
			if (!error)
				error = (write(fd, argv[i], strlen(argv[i])) == -1);
			if (error) {
				close(fd);
				perror(argv[1]);
				return 1;
			}
		}
	}

	if (close(fd) == -1) {
		perror(argv[1]);
		return 1;
	}

	return 0;
}
