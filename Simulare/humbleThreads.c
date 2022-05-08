#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

long long sum;
int numberOfThreads;
sem_t semaphore;

void P(sem_t *semaphore) {
    sem_wait(semaphore);
}
void V(sem_t *semaphore) {
    sem_post(semaphore);
}

void computeSum(int* th_id) {
    printf("Thread ID: %d\n", *th_id);
    for (int i = numberOfThreads * (*th_id) + 1; i <= numberOfThreads * (*th_id + 1); i++) {
        P(&semaphore);
        sum += i;
        V(&semaphore);
    }
}

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("USAGE: %s <number_of_threads>\n", argv[0]);
        exit(1);
    }

    sscanf(argv[1], "%d", &numberOfThreads);

    if (numberOfThreads <= 0) {
        printf("ERROR: The number of threads must be greater than 0\n");
        exit(1);
    }

    if (sem_init(&semaphore, 1, 1) < 0) {
        perror("ERROR: Couldn't create the semaphore set");
        exit(1);
    }

    pthread_t *threads = (pthread_t *)malloc(numberOfThreads * sizeof(pthread_t));
    for (int i = 0; i < numberOfThreads; i++) {
        int* th_id = (int *)malloc(sizeof(int));
        *th_id = i;
        if (pthread_create(&threads[i], NULL, (void* (*) (void*)) computeSum, th_id) != 0) {
            perror("ERROR: Couldn't create the threads");
            exit(3);
        }
    }

    for (int i = 0; i < numberOfThreads; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Main thread Sum: %lld\n", sum);
    sem_destroy(&semaphore);
    return 0;
}