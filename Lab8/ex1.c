#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

long long count = 0;
sem_t semaphore;

void P(sem_t *semaphore) {
    sem_wait(semaphore);
}
void V(sem_t *semaphore) {
    sem_post(semaphore);
}

void increment(int *th_id) {
    P(&semaphore);

    long long aux = count;
    aux++;
    // usleep(10000);
    count = aux;

    V(&semaphore);
}

int main(int argc, char **argv) {
   if (argc != 2) {
        printf("USAGE: %s <number_of_threads>\n", argv[0]);
        exit(1);
    }
    int numberOfThreads;
    sscanf(argv[1], "%d", &numberOfThreads);
    if (numberOfThreads <= 0) {
        printf("ERROR: The number of threads must be greater than 0\n");
        exit(1);
    }

    if (sem_init(&semaphore, 1, 1) < 0) {
        perror("ERROR: Couldn't create the semaphore set");
        exit(2);
    }

    pthread_t *threads = (pthread_t *)malloc(numberOfThreads * sizeof(pthread_t));
    for (int i = 0; i < numberOfThreads; i++) {
        if (pthread_create(&threads[i], NULL, (void* (*) (void*)) increment, &i) != 0) {
            perror("ERROR: Couldn't create the threads");
            exit(3);
        }
    }

    for (int i = 0; i < numberOfThreads; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Count: %lld\n", count);
    sem_destroy(&semaphore);
    return 0;
}
