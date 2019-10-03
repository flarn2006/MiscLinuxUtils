#define _GNU_SOURCE
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s PROGRAM [ARGS...]\n", argv[0]);
        return 255;
    }

    pid_t forkpid = fork();
    if (forkpid) {
        /* Parent process */
        int wstatus;
        waitpid(forkpid, &wstatus, 0);
        if (WIFSTOPPED(wstatus) && WSTOPSIG(wstatus) == SIGTRAP) {
            kill(forkpid, SIGSTOP);
            ptrace(PTRACE_DETACH, forkpid);
            printf("%d\n", forkpid);
            return 0;
        } else if (WIFEXITED(wstatus)) {
            return WEXITSTATUS(wstatus);
        } else {
            return 254;
        }
    } else {
        /* Child process */
        if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) == -1) {
            perror("ptrace");
        } else {
            execvp(argv[1], argv+1);
            perror(argv[1]);
        }
        return 254;
    }
}