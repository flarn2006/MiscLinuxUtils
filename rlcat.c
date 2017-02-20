#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>

int main(int argc, char *argv[])
{
	int fd = dup(2);
	dup2(1, 2);
	dup2(fd, 1);
	close(fd);

	rl_bind_key('\t', rl_insert);

	const char *prompt = "";
	if (argc > 1)
		prompt = argv[1];
	
	char *line;
	while ((line = readline(prompt)) != NULL) {
		fprintf(stderr, "%s\n", line);

		static const char *lastline = "";
		if (line[0] != '\0' && strcmp(line, lastline)) {
			add_history(line);
			if (lastline[0] != '\0')
				free((char*)lastline);
			lastline = line;
		} else {
			free(line);
		}
	}

	return 0;
}
