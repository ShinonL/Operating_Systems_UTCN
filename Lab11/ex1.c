#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>

int fdP12[2], fdP21[2];

void increment1(int pid, long long MAX) {
    long long count = 0;
    char buff;
    for (count = 0; count <= MAX; count++) {
        printf("Process %d at step %lld\n", pid, count);
        write(fdP21[1], "1", 1);
        read(fdP12[0], &buff, 1);
    }
}

void increment2(int pid, long long MAX) {
    long long count = 0;
    char buff;
    for (count = 0; count <= MAX; count++) {
        read(fdP21[0], &buff, 1);
        printf("Process %d at step %lld\n", pid, count);
        write(fdP12[1], "2", 1);
    }
}

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("USAGE: %s <MAX>\n", argv[0]);
        exit(1);
    }
    
    long long MAX;
    sscanf(argv[1], "%lld", &MAX);

    if (MAX < 0) {
        printf("MAX should be a positive integer\n");
        exit(1);
    }

    pipe(fdP12);
    pipe(fdP21);
    
    int p2;

    int p1 = fork();
    switch (p1) {
    case 0:
        close(fdP21[0]);
        close(fdP12[1]);

        increment1(getpid(), MAX);

        break;
    
    default:
        p2 = fork();
        switch (p2) {
        case 0:
            close(fdP21[1]);
            close(fdP12[0]);

            increment2(getpid(), MAX);

            break;

        default:
            waitpid(p1, NULL, 0);
            waitpid(p2, NULL, 0);
            break;
        }

        break;
    }
    return 0;
}
