#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>

int pipe1[2], pipe2[2];

int main(int argc, char** argv) {
    pipe(pipe1);
    pipe(pipe2);

    int pid = fork();
    if (pid == 0) {
        char message[22];

        read(pipe1[0], message, 21);
        message[21] = '\0';
        printf("[Child] %s\n", message);

        write(pipe2[1], "Sol-Do-La-Ti-Do-Re-Do", 21);
    } else {
        write(pipe1[1], "Sol-Do-La-Fa-Me-Do-Re", 21);

        char message[22];

        read(pipe2[0], message, 21);
        message[21] = '\0';
        printf("[Parent] %s\n", message);
    }

    return 0;
}