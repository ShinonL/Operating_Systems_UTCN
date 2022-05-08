#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

long long count = 0;
sem_t *semaphores;
int numberOfThreads;

void P(sem_t *semaphore) {
    sem_wait(semaphore);
}
void V(sem_t *semaphore) {
    sem_post(semaphore);
}

void increment(int *th_id) {
    int i = 0;
    while (i < 100) {
        P(&semaphores[((*th_id) - 1) % numberOfThreads]);

        long long aux = count;
        aux++;
        printf("count = %lld from thread %d\n", count, *th_id);
        count = aux;

        V(&semaphores[(*th_id) % numberOfThreads]);
        i++;
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("USAGE: %s <number_of_threads>\n", argv[0]);
        exit(1);
    }

    sscanf(argv[1], "%d", &numberOfThreads);
    if (numberOfThreads <= 0) {
        printf("ERROR: The number of threads must be greater than 0\n");
        exit(1);
    }

    semaphores = (sem_t *)malloc(numberOfThreads * sizeof(sem_t));

    if (sem_init(&semaphores[0], 1, 1) < 0) {
        perror("ERROR: Couldn't create the semaphore set");
        exit(2);
    }
    for (int i = 1; i < numberOfThreads; i++) {
        if (sem_init(&semaphores[i], 1, 0) < 0) {
            perror("ERROR: Couldn't create the semaphore set");
            exit(2);
        }
    }

    pthread_t *threads = (pthread_t *)malloc(numberOfThreads * sizeof(pthread_t));
    for (int i = 0; i < numberOfThreads; i++) {
        int* th_id = (int *)malloc(sizeof(int));
        *th_id = i + 1;
        if (pthread_create(&threads[i], NULL, (void* (*) (void*)) increment, th_id) != 0) {
            perror("ERROR: Couldn't create the thread 1");
            exit(3);
        }
        // free(th_id);
    }

    for (int i = 0; i < numberOfThreads; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("count = %lld [THE END]\n", count);

    for (int i = 0; i < numberOfThreads; i++) {
        sem_destroy(&semaphores[i]);
    }
    return 0;
}
