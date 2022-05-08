#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

pthread_mutex_t lock;
pthread_cond_t cond;

int numberOfCannibals;
int numberOfMissionaries;
int peopleOnShore;

typedef struct {
    int ID;
    short role;
} ThreadInfoT;

void crossingRiver(void* arg) {
    ThreadInfoT threadInfo = *((ThreadInfoT*) arg);

    if (pthread_mutex_lock(&lock) != 0) {
        perror("ERROR: Couldn't take the lock");
        exit(3);
    }

    while(numberOfCannibals + numberOfMissionaries == 3 ||
            (peopleOnShore != 1 && numberOfCannibals == 2 && threadInfo.role == 0) ||
            (peopleOnShore != 1 && numberOfCannibals == 1 && numberOfMissionaries == 1 && threadInfo.role == 1)) {
        printf("cann %d\n", numberOfCannibals);
        if (pthread_cond_wait(&cond, &lock) != 0) {
            perror("ERROR: Couldn't wait for condition");
            exit(4);
        }
    }

    if (threadInfo.role == 0) {
        printf("Enterted the Boat: Person %d Role: missionary\n", threadInfo.ID);
        numberOfMissionaries++;
    } else {
        printf("Enterted the Boat: Person %d Role: cannibal\n", threadInfo.ID);
        numberOfCannibals++;
    }
    peopleOnShore--;

    if(numberOfMissionaries == 2 && numberOfCannibals == 1)
        printf("Boat left with 2 misionaries and 1 canibal !\n");
    else if(numberOfMissionaries == 3 && numberOfCannibals == 0)
        printf("Boat left with 3 misionaries and 0 canibals !\n");
    else if( numberOfMissionaries == 0 && numberOfCannibals == 3)
        printf("Boat left with 0 misionaries and 3 canibals !\n");
    else if(numberOfMissionaries + numberOfCannibals ==3)
        printf("Boat can not departure. too bad missionaries, u're gonna be dinner !\n");

    if (numberOfMissionaries + numberOfCannibals == 3) {
        numberOfMissionaries = 0;
        numberOfCannibals = 0;
    }

    if (pthread_cond_broadcast(&cond)  != 0) {
        perror("ERROR: Couldn't signal the condition waiters");
        exit(4);
    }

    if (pthread_mutex_unlock(&lock) != 0) {
        perror("ERROR: Couldn't release the lock");
        exit(3);
    }
}

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("USAGE: %s <number_of_threads>\n", argv[0]);
        exit(1);
    }

    int numberOfThreads;
    sscanf(argv[1], "%d", &numberOfThreads);
    if(numberOfThreads <= 0 || numberOfThreads % 3 != 0) {
        printf("ERROR: Please insert a number of threads greater than 0 AND divisible by 3\n");
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

    peopleOnShore = numberOfThreads;

    pthread_t *threads = (pthread_t *)malloc(numberOfThreads * sizeof(pthread_t));
    ThreadInfoT* threadsInfo = (ThreadInfoT *)malloc(numberOfThreads * sizeof(ThreadInfoT));
    srand(time(NULL));
    for (int i = 0; i < numberOfThreads; i++) {
        threadsInfo[i].ID = i + 1;
        threadsInfo[i].role = rand()%2;
        if (pthread_create(&threads[i], NULL, (void* (*) (void*)) crossingRiver,  &threadsInfo[i]) != 0) {
            perror("ERROR: Couldn't create threads");
            exit(2);
        }
    }

    for (int i = 0; i < numberOfThreads; i++) {
        pthread_join(threads[i], NULL);
    }

    if (pthread_mutex_destroy(&lock) != 0) {
    	perror("ERROR: Couldn't destroy the lock");
    	exit(2);
    }

    if (pthread_cond_destroy(&cond) != 0) {
    	perror("ERROR: Couldn't destroy the condition variable");
    	exit(2);
    }

    return 0;
}
