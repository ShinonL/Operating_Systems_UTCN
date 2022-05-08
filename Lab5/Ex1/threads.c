#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

long long count = 0;
sem_t *semaphores;

void P(sem_t *semaphore) {
    sem_wait(semaphore);
}
void V(sem_t *semaphore) {
    sem_post(semaphore);
}


void increment(int th_id) {
    int i = 0;
    while (i < 100) {
        P(&semaphores[(th_id - 1) % 2]);

        long long aux = count;
        aux++;
        printf("count = %lld from thread %d\n", count, th_id);
        count = aux;

        V(&semaphores[th_id % 2]);
        i++;
    }
}

int main(int argc, char **argv) {
    if (argc != 1) {
        printf("USAGE: %s\n", argv[0]);
        exit(1);
    }

    semaphores = (sem_t *)malloc(2 * sizeof(sem_t));

    if (sem_init(&semaphores[0], 1, 1) < 0) {
        perror("ERROR: Couldn't create the semaphore set");
        exit(2);
    }
    for (int i = 1; i < 2; i++) {
        if (sem_init(&semaphores[i], 1, 0) < 0) {
            perror("ERROR: Couldn't create the semaphore set");
            exit(2);
        }
    }

    pthread_t thread_1, thread_2;

    if (pthread_create(&thread_1, NULL, (void* (*) (void*)) increment, (void*) 1) != 0) {
        perror("ERROR: Couldn't create the thread 1");
        exit(3);
    }
    if (pthread_create(&thread_2, NULL, (void* (*) (void*)) increment, (void*) 2) != 0) {
        perror("ERROR: Couldn't create the thread 2");
        exit(3);
    }

    pthread_join(thread_1, NULL);
    pthread_join(thread_2, NULL);

    printf("count = %lld [THE END]\n", count);
    for (int i = 0; i < 2; i++) {
        sem_destroy(&semaphores[i]);
    }
    return 0;
}