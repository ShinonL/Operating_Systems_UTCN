#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <wait.h>

int numberOfProcesses;
int** fdPipes;

void increment(int pid, long long MAX) {
    long long count = 0;
    char buff;
    for (count=0; count<=MAX; count++) {
        read(fdPipes[pid][0], &buff, 1);
        printf("Process %d at step %lld\n", getpid(), count);
        write(fdPipes[(pid + 1) % numberOfProcesses][1], "1", 1);
    }
}
int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("USAGE: %s <number_of_processes> <MAX>\n", argv[0]);
        exit(1);
    }
    
    sscanf(argv[1], "%d", &numberOfProcesses);
    if (numberOfProcesses < 2) {
        printf("The number of processes should greater than 1\n");
        exit(1);
    }

    long long MAX;
    sscanf(argv[2], "%lld", &MAX);

    if (MAX < 0) {
        printf("MAX should be a positive integer\n");
        exit(1);
    }

    fdPipes = (int **)malloc(numberOfProcesses * sizeof(int *));
    for (int i = 0; i < numberOfProcesses; i++) {
        fdPipes[i] = (int *)malloc(2 * sizeof(int));
        pipe(fdPipes[i]);
    }

    write(fdPipes[0][1], "0", 1);
    for (int i = 0; i < numberOfProcesses; i++) {
        if (fork() == 0) {
            // printf("process %d , parent %d\n",getpid(),getppid());
            increment(i, MAX);
            return 0;
        }
    }

    for (int i = 0; i < numberOfProcesses; i++)
        wait(NULL);

    return 0;
}
