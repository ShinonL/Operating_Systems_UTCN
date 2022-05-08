// It does not extract the cars in the order they arrived at the queue of a traffic light
// I didn't figure how to do that to the threads because I have no control on which threads is going to be woken up first

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

pthread_mutex_t lock;
pthread_cond_t cond;

typedef struct {
    int ID;
    short streetNumber;
} ThreadInfoT;

bool blocked = false;

enum Light{ RED, GREEN } lights[2];

void crossingCar(void* arg) {
    ThreadInfoT threadInfo = *((ThreadInfoT *)arg);

    printf("Intersection %d: Car %d arrived\n", threadInfo.streetNumber, threadInfo.ID);

    if (pthread_mutex_lock(&lock) != 0) {
        perror("ERROR: Couldn't take the lock");
        exit(3);
    }

    while (lights[threadInfo.streetNumber] == RED || blocked){
        // printf("here\n");
        if (pthread_cond_wait(&cond, &lock) != 0) {
            perror("ERROR: Couldn't wait for condition");
            exit(4);
        }
    }
        
    printf("Intersection %d: Car %d crossing\n", threadInfo.streetNumber, threadInfo.ID);
    
    blocked = true;

    if (pthread_mutex_unlock(&lock) != 0) {
        perror("ERROR: Couldn't release the lock");
        exit(3);
    }

    usleep(1000);

    if (pthread_mutex_lock(&lock) != 0) {
        perror("ERROR: Couldn't take the lock");
        exit(3);
    }

    blocked = false;

    printf("Intersection %d: Car %d crossed\n", threadInfo.streetNumber, threadInfo.ID);
    // if (pthread_cond_broadcast(&cond)  != 0) {
    // 	perror("ERROR: Couldn't signal the condition");
    // 	exit(4);
    // }

    if (pthread_mutex_unlock(&lock) != 0) {
        perror("ERROR: Couldn't release the lock");
        exit(3);
    }
    
}

void changeTrafficLight() {
    while(1) {
        if (pthread_mutex_lock(&lock) != 0) {
            perror("ERROR: Couldn't take the lock");
            exit(3);
        }

        lights[0] = GREEN;
        lights[1] = RED;
        if (pthread_cond_signal(&cond)  != 0) {
            perror("ERROR: Couldn't signal the condition");
            exit(4);
        }

        if (pthread_mutex_unlock(&lock) != 0) {
            perror("ERROR: Couldn't release the lock");
            exit(3);
        }

        usleep(100);



        if (pthread_mutex_lock(&lock) != 0) {
            perror("ERROR: Couldn't take the lock");
            exit(3);
        }

        lights[0] = RED;
        lights[1] = GREEN;
        if (pthread_cond_signal(&cond)  != 0) {
            perror("ERROR: Couldn't signal the condition");
            exit(4);
        }

        if (pthread_mutex_unlock(&lock) != 0) {
            perror("ERROR: Couldn't release the lock");
            exit(3);
        }

        usleep(100);
    }
}

int main(int argc, char** argv) {
    if(argc != 2){
        printf("USAGE: %s <number_of_threads>\n", argv[0]);
        exit(1);
    }

    int numberOfThreads;
    sscanf(argv[1], "%d", &numberOfThreads);
    if(numberOfThreads <= 0) {
        printf("ERROR: Please insert a number of threads greater than 0\n");
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
    ThreadInfoT* threadsInfo = (ThreadInfoT *)malloc(numberOfThreads * sizeof(ThreadInfoT));
    for (int i = 0; i < numberOfThreads; i++) {
        threadsInfo[i].ID = i + 1;
        threadsInfo[i].streetNumber = rand() % 2;
	    if (pthread_create(&threads[i], NULL, (void* (*) (void*)) crossingCar, (void*)&threadsInfo[i])) {
            perror("ERROR: Couldn't create the threads");
            exit(2);
        }
    }

    pthread_t lightsThread;
    if (pthread_create(&lightsThread, NULL, (void* (*) (void*)) changeTrafficLight, NULL)) {
        perror("ERROR: Couldn't create traffic light thread");
        exit(2);
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