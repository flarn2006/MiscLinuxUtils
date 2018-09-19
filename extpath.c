#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr, "Usage: %s cmd [args...]\n", argv[0]);
		return 255;
	}
	
	char *pathlist = getenv("EXTPATH");
	char *pathbuf = NULL;
	if (pathlist) {
		pathbuf = malloc(strlen(pathlist) + strlen(argv[1]) + 2);
		char *dir = strtok(pathlist, ":");
		while (dir) {
			strcpy(pathbuf, dir);
			if (dir[strlen(dir) - 1] != '/')
				strcat(pathbuf, "/");
			strcat(pathbuf, argv[1]);
			execv(pathbuf, argv+1);
			dir = strtok(NULL, ":");
		}
	} else {
		fprintf(stderr, "%s: warning: no EXTPATH environment variable found\n", argv[0]);
	}

	execvp(argv[1], argv+1);

	if (pathbuf)
		free(pathbuf);
	fprintf(stderr, "%s: %s: command not found\n", argv[0], argv[1]);
	return 254;
}
