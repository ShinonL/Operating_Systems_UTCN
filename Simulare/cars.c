#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t semaphore1;
int no_cars;

void P(sem_t *semaphore) {
    sem_wait(semaphore);
}
void V(sem_t *semaphore) {
    sem_post(semaphore);
}

void car_parking(void* arg) {
    P(&semaphore1);

    no_cars++;

    printf("No of cars in the park: %d\n", no_cars);
    usleep(2000);

    no_cars--;

    V(&semaphore1);
}

int main(int argc, char** argv) {
    if (sem_init(&semaphore1, 0, 50) < 0) {
        perror("ERROR: Couldn't create the semaphore set");
        exit(1);
    }

    pthread_t *threads = (pthread_t *)malloc(1000 * sizeof(pthread_t));
    for (int i = 0; i < 1000; i++) {
        usleep(5);
        int* th_id = (int *)malloc(sizeof(int));
        *th_id = i;
        if (pthread_create(&threads[i], NULL, (void* (*) (void*)) car_parking, th_id) != 0) {
            perror("ERROR: Couldn't create the threads");
            exit(3);
        }
    }

    for (int i = 0; i < 1000; i++) {
        pthread_join(threads[i], NULL);
    }

    sem_destroy(&semaphore1);
    return 0;
}