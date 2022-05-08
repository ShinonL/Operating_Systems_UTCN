#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char** argv)  {
    if (argc != 2) {
        printf("USAGE: %s <ls_parameter>\n", argv[0]);
        exit(1);
    }

    int status;
    int pid = fork();
    if (pid == 0) {
        printf("[Child] Own PID: %d Parent PID: %d No child\n", getpid(), getppid());
        execlp("ls", "ls", argv[1], NULL);
    } else {
        waitpid(pid, &status, 0);
        printf("[Parent] Own PID: %d Parent PID: %d Child PID: %d\n", getpid(), getppid(), pid);
        printf("[Parent] Child exit status %d\n", status);
    }

    return 0;
}