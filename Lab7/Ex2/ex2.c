#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include <sys/types.h>
#include <unistd.h>

#include <pthread.h>

char stop;

void* executeThread(void* ID) {
    int currentID = *((int *) ID);
    printf("[Thread %d] Starting. tid=%ld, pid=%d\n", currentID, (long int)pthread_self(), (int)getpid());
    
    while (stop != 'q') {
        sleep(1);
        printf("[Thread %d] Unique Id = %d TID = %ld\n", currentID, currentID, (long int)pthread_self());
    }

    printf("[Thread %d] Terminating.\n", currentID);

    return ID;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("USAGE: ./ex1 <number_of_threads>\n");
        exit(1);
    }
    
    printf("[Thread MAIN] Starting. TID = %ld PID = %d\n", (long int)pthread_self(), (int)getpid());

    int numberOfThreads = atoi(argv[1]);
    printf("num: %d\n", numberOfThreads);

    pthread_t* threads = (pthread_t *)malloc(numberOfThreads * sizeof(pthread_t));
    int** states = (int **)malloc(numberOfThreads * sizeof(int *));
    int* IDs = (int *)malloc(numberOfThreads * sizeof(int));

    for(int i = 0; i < numberOfThreads; i++) {
        IDs[i] = i + 1;
        if (pthread_create(&threads[i], NULL, executeThread, &IDs[i]) != 0) {
            perror("Error creating a new thread");
            exit(1);
        }
        printf("[Thread MAIN] Created thread %d. TID = %ld\n", IDs[i], (long int)threads[i]);
    }
    printf("[Thread MAIN] Created all threads\n");

    while (scanf("%c", &stop)) {
        for(int i = 0; i < numberOfThreads; i++) {
            pthread_join(threads[i], (void**) &states[i]);
        }

        if (stop == 'q') break;
    }
    
    printf("[Thread MAIN] Terminating.\n");
    return 0;
}