#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

int th_no = 0;
sem_t semaphore, semaphore_M;

void P(sem_t *semaphore) {
    sem_wait(semaphore);
}
void V(sem_t *semaphore) {
    sem_post(semaphore);
}

void limited_area(void) {
    P(&semaphore_M);

    P(&semaphore);
    th_no++;
    V(&semaphore);

    // usleep(1000); // comment at 3.
    printf("The no of threads in the limited area is: %d\n", th_no);

    P(&semaphore);
    th_no--;
    V(&semaphore);

    V(&semaphore_M);
}

int main(int argc, char **argv) {
    if(argc != 3) {
        printf("USAGE: %s <number_of_threads> <number_M>\n", argv[0]);
        exit(1);
    }
    int numberOfThreads;
    sscanf(argv[1], "%d", &numberOfThreads);
    if (numberOfThreads <= 0) {
        printf("ERROR: The number of threads must be greater than 0\n");
        exit(1);
    }
    int numberM;
    sscanf(argv[2], "%d", &numberM);
    if (numberM <= 0 || numberM >= numberOfThreads) {
        printf("ERROR: The number M must be greater than 0 and smaller then the number of threads\n");
        exit(1);
    }

    if (sem_init(&semaphore, 1, 1) < 0) {
        perror("ERROR: Couldn't create the semaphore set");
        exit(2);
    }
    if (sem_init(&semaphore_M, 1, numberM) < 0) {
        perror("ERROR: Couldn't create the semaphore set");
        exit(2);
    }

    pthread_t *threads = (pthread_t *)malloc(numberOfThreads * sizeof(pthread_t));
    for (int i = 0; i < numberOfThreads; i++) {
        if (pthread_create(&threads[i], NULL, (void* (*) (void*)) limited_area, NULL) != 0) {
            perror("ERROR: Couldn't create the threads");
            exit(3);
        }
    }

    for (int i = 0; i < numberOfThreads; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Thread no = %d\n", th_no);
    sem_destroy(&semaphore);
    sem_destroy(&semaphore_M);
    return 0;
}
