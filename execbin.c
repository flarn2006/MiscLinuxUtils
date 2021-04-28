#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <signal.h>

typedef long (*func)(void);

int main(int argc, char* argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s binfile\n", argv[0]);
		return 255;
	}

	const char* filename = argv[1];

	int fd = open(filename, O_RDONLY);
	if (fd == -1) {
		perror(filename);
		return 254;
	}

	int retval = 0;

	struct stat st;
	if (fstat(fd, &st) == -1) {
		perror(filename);
		retval = 254;
		goto exit_close_file;
	}

	void* map = mmap(NULL, st.st_size, PROT_READ|PROT_WRITE|PROT_EXEC, MAP_PRIVATE, fd, 0);
	if (map == MAP_FAILED) {
		perror("mmap");
		retval = 254;
		goto exit_close_file;
	}

	signal(SIGTRAP, SIG_IGN);
	raise(SIGTRAP);

	long result;
#ifdef DONT_CLEAR_RAX
	result = ((func)map)();
#else
	asm volatile(
		"xor	%%rax, %%rax\n"
		"call	*%1\n"
		"mov	%%rax, %0"
		: "=m"(result)
		: "b"(map)
		: "rax", "rcx", "rdx", "cc", "memory"
	);
#endif

	printf("Code returned 0x%016lX (%ld)\n", result, result);
	retval = (int)(result & 0xFF);

	munmap(map, st.st_size);

exit_close_file:
	close(fd);
	return retval;
}
