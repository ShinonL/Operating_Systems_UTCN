#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t lock;
pthread_cond_t cond;

long long count = 0;
int numberOfThreads, numberOfRepetitions;
int nextThread = 1;

void increment(void* arg) {
    long long aux;
    int th_id = *((int*) arg);

    for (int i = 0; i < numberOfRepetitions; i++) {
        if (pthread_mutex_lock(&lock) != 0) {
            perror("ERROR: Couldn't take the lock");
            exit(3);
        }

        while(th_id != nextThread) {
            if (pthread_cond_wait(&cond, &lock) != 0) {
                perror("ERROR: Couldn't wait for condition");
                exit(3);
            }
        }

        if (pthread_mutex_unlock(&lock) != 0) {
            perror("ERROR: Couldn't release the lock");
            exit(3);
        }


        aux = count;
        printf("Thread %d read count having value %lld\n", th_id, aux);
        aux++;
        usleep(100);
        printf("Thread %d will update count with value %lld\n", th_id, aux);
        count = aux;

        if (pthread_mutex_lock(&lock) != 0) {
            perror("ERROR: Couldn't take the lock");
            exit(3);
        }

        nextThread = ((th_id) % numberOfThreads) + 1;

        if (pthread_cond_broadcast(&cond)  != 0) {
            perror("ERROR: Couldn't signal the condition waiters");
            exit(3);
        }

        if (pthread_mutex_unlock(&lock) != 0) {
            perror("ERROR: Couldn't release the lock");
            exit(3);
        }
    }
}

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("USAGE: %s <number_of_threads> <number_of_repetitions>\n", argv[0]);
        exit(1);
    }

    sscanf(argv[1], "%d", &numberOfThreads);
    if(numberOfThreads <= 0) {
        printf("ERROR: Please insert a number of threads greater than 0\n");
        exit(1);
    }

    sscanf(argv[2], "%d", &numberOfRepetitions);
    if(numberOfRepetitions <= 0) {
        printf("ERROR: Please insert a number of repetitions greater than 0\n");
        exit(1);
    }

    if (pthread_mutex_init(&lock, NULL) != 0) {
    	perror("ERROR: Couldn't initialize the lock");
    	exit(2);
    }

    if (pthread_cond_init(&cond, NULL) != 0) {
    	perror("ERROR: Couldn't initialize the condition variable");
    	exit(2);
    }

    pthread_t *threads = (pthread_t *)malloc(numberOfThreads * sizeof(pthread_t));
    int* threadIDs = (int *)malloc(numberOfThreads * sizeof(numberOfThreads));

    for (int i = 0; i < numberOfThreads; i++) {
        threadIDs[i] = i + 1;
        if (pthread_create(&threads[i], NULL, (void* (*) (void*)) increment,  &threadIDs[i]) != 0){
            perror("ERROR: Couldn't create the threads");
            exit(2);
        }
    }

    for (int i = 0; i < numberOfThreads; i++) {
        pthread_join(threads[i], NULL);
    }

    if (pthread_mutex_destroy(&lock) != 0) {
    	perror("ERROR: Couldn't destroy the lock");
    	exit(4);
    }

    if (pthread_cond_destroy(&cond) != 0) {
    	perror("ERROR: Couldn't destroy the condition variable");
    	exit(4);
    }
    return 0;
}
