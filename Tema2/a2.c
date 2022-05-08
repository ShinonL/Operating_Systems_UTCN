#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <fcntl.h>
#include "a2_helper.h"

void P(sem_t *semaphore) {
    sem_wait(semaphore);
}
void V(sem_t *semaphore) {
    sem_post(semaphore);
}

const char *semaphoreName1 = "Thr83_Thr52";
const char *semaphoreName2 = "Thr85_Thr52";
sem_t *semaphore_t83_t52, *semaphore_t85_t52;

// ------------ Same Proccess Thread Synchronization ------------ //
sem_t semaphoresT1Started, semaphoresT4Ended;

void runThreadsForP5(void* arg) {
    int threadID = *(int *)arg;

    // force T5.4 to wait until T5.1 starts
    if (threadID == 4)
        P(&semaphoresT1Started);

    // force T5.2 to wait until T8.3 ends
    if (threadID == 2)
        P(semaphore_t83_t52);

    info(BEGIN, 5, threadID);

    // force T5.1 to start T5.4 and wait until it ends
    if(threadID == 1) {
        V(&semaphoresT1Started);
        P(&semaphoresT4Ended);
    }
    
    info(END, 5, threadID);
    
    // force T5.2 to start T8.5
    if (threadID == 2)
        V(semaphore_t85_t52);

    // force T5.4 to start T5.1
    if (threadID == 4)
        V(&semaphoresT4Ended);
}

void runProccess5() {
    if (sem_init(&semaphoresT1Started, 0, 0) < 0) {
        perror("ERROR: Couldn't create a semaphore set");
        exit(2);
    }
    if (sem_init(&semaphoresT4Ended, 0, 0) < 0) {
        perror("ERROR: Couldn't create a semaphore set");
        exit(2);
    }

    pthread_t* threads = (pthread_t *)malloc(4 * sizeof(pthread_t));
    int* threadIDs = (int *)malloc(4 * sizeof(int));

    for (int i = 0; i < 4; i++) {
        threadIDs[i] = i + 1;
        if (pthread_create(&threads[i], NULL, (void* (*) (void*)) runThreadsForP5, &threadIDs[i]) != 0) {
            perror("ERROR: Couldn't create the threads for proccess 5");
            exit(2);
        }
    }

    for (int i = 0; i < 4; i++) {
        pthread_join(threads[i], NULL);
    }

    free(threads);
    free(threadIDs);

    sem_destroy(&semaphoresT1Started);
    sem_destroy(&semaphoresT4Ended);
}

// ------------ Threads Barrier ------------ //
sem_t semaphore1, semaphore2, semaphore_max6, semaphore_barrier, semaphore_count;
int count;

void runThreadsForP9(void* arg) {
    int threadID = *(int *)arg;
    int localCount;
    
    // make sure T10 is in the first 6 threads that start
    if (threadID != 10) {
        P(&semaphore1);
        V(&semaphore1);

        // do not allow more than 6 at a time
        P(&semaphore_max6);
        info(BEGIN, 9, threadID);
    } else {
        // do not allow more than 6 at a time
        P(&semaphore_max6);
        info(BEGIN, 9, threadID);

        V(&semaphore1);
    }

    // count how many have started until now
    P(&semaphore_count);
    count++;
    localCount = count;
    V(&semaphore_count);

    // if less than six, use a barrier to stop them
    // if exacly six, let them pass throusgh the barrier
    // if more, ignore the barrier
    if (localCount < 6) {
        P(&semaphore_barrier);
        V(&semaphore_barrier);
    } else if (localCount == 6)
        V(&semaphore_barrier);

    // for the first 6, make sure that T10 is the first to end
    if (threadID != 10) {
        P(&semaphore2);
        info(END, 9, threadID);
        V(&semaphore2);
    } else {
        info(END, 9, threadID);
        V(&semaphore2);
    }

    V(&semaphore_max6);
}

void runProccess9() {
    if (sem_init(&semaphore1, 0, 0) < 0) {
        perror("ERROR: Couldn't create a semaphore set");
        exit(2);
    }
    if (sem_init(&semaphore2, 0, 0) < 0) {
        perror("ERROR: Couldn't create a semaphore set");
        exit(2);
    }
    if (sem_init(&semaphore_barrier, 0, 0) < 0) {
        perror("ERROR: Couldn't create a semaphore set");
        exit(2);
    }
    if (sem_init(&semaphore_max6, 0, 6) < 0) {
        perror("ERROR: Couldn't create a semaphore set");
        exit(2);
    }
    if (sem_init(&semaphore_count, 0, 1) < 0) {
        perror("ERROR: Couldn't create a semaphore set");
        exit(2);
    }

    pthread_t* threads = (pthread_t *)malloc(49 * sizeof(pthread_t));
    int* threadIDs = (int *)malloc(49 * sizeof(int));

    for (int i = 0; i < 49; i++) {
        threadIDs[i] = i + 1;
        if (pthread_create(&threads[i], NULL, (void* (*) (void*)) runThreadsForP9, &threadIDs[i]) != 0) {
            perror("ERROR: Couldn't create the threads for proccess 9");
            exit(2);
        }
    }

    for (int i = 0; i < 49; i++) {
        pthread_join(threads[i], NULL);
    }

    free(threads);
    free(threadIDs);

    sem_destroy(&semaphore1);
    sem_destroy(&semaphore2);
    sem_destroy(&semaphore_max6);
    sem_destroy(&semaphore_barrier); 
    sem_destroy(&semaphore_count);
}

// ------------ Different Proccesses Thread Synchronization ------------ //

void runThreadsForP8(void *arg) {
    int threadID = *(int *)arg;

    // force T8.5 to wait until T5.2 ends
    if (threadID == 5)
        P(semaphore_t85_t52);

    info(BEGIN, 8, threadID);
    info(END, 8, threadID);

    // force T8.3 to start T5.2
    if (threadID == 3)
        V(semaphore_t83_t52);
}

void runProccess8() {
    pthread_t* threads = (pthread_t *)malloc(6 * sizeof(pthread_t));
    int* threadIDs = (int *)malloc(6 * sizeof(int));

    for (int i = 0; i < 6; i++) {
        threadIDs[i] = i + 1;
        if (pthread_create(&threads[i], NULL, (void* (*) (void*)) runThreadsForP8, &threadIDs[i]) != 0) {
            perror("ERROR: Couldn't create the threads for proccess 9");
            exit(2);
        }
    }

    for (int i = 0; i < 6; i++) {
        pthread_join(threads[i], NULL);
    }

    free(threads);
    free(threadIDs);
}

// ------------ Proccess Hierarchy ------------ //
int main(){
    // initialize the tester
    init();

    // common semaphores between P5 and P8 
    // initialized here because we don't know which proccess starts first
    semaphore_t83_t52 = sem_open(semaphoreName1, O_CREAT, 0600, 0);
    semaphore_t85_t52 = sem_open(semaphoreName2, O_CREAT, 0600, 0);
    sem_unlink(semaphoreName1);
    sem_unlink(semaphoreName2);

    // print info about starting P1
    info(BEGIN, 1, 0);
    
    int pid2 = fork();
    switch (pid2) {
    case -1:
        perror("ERROR: Couldn't create a new child");
        exit(1);
        break;
    case 0:
        // print info about starting P2
        info(BEGIN, 2, 0);

        // create P5
        int pid5 = fork();
        switch (pid5) {
        case -1:
            perror("ERROR: Couldn't create a new child");
            exit(1);
            break;
        case 0:
            // print info about starting P5
            info(BEGIN, 5, 0);

            runProccess5();

            // print info about ending P5
            info(END, 5, 0);
            break;
        default:
            // wait P5 to end
            waitpid(pid5, NULL, 0);
            
            // print info about ending P2
            info(END, 2, 0);
            break;
        }
        break;
    default: {
        // inside P1
        // create P3
        int pid3 = fork();
        switch (pid3) {
        case -1:
            perror("ERROR: Couldn't create a new child");
            exit(1);
            break;
        case 0:
            // print info about starting P3
            info(BEGIN, 3, 0);

            // create P4
            int pid4 = fork();
            switch (pid4) {
            case -1:
                perror("ERROR: Couldn't create a new child");
                exit(1);
                break;
            case 0:
                // print info about starting P4
                info(BEGIN, 4, 0);

                // create P9
                int pid9 = fork();
                switch (pid9) {
                case -1:
                    perror("ERROR: Couldn't create a new child");
                    exit(1);
                    break;
                case 0:
                    // print info about starting P9
                    info(BEGIN, 9, 0);

                    runProccess9();

                    // print info about ending P9
                    info(END, 9, 0);
                    break;
                default:
                    // wait P9 to end
                    waitpid(pid9, NULL, 0);
                    
                    // print info about ending P4
                    info(END, 4, 0);
                    break;
                }
                break;
            default: {
                // inside P3
                // create P6
                int pid6 = fork();
                switch (pid6) {
                case -1:
                    perror("ERROR: Couldn't create a new child");
                    exit(1);
                    break;
                case 0:
                    // print info about starting P6
                    info(BEGIN, 6, 0);
                    // print info about ending P6
                    info(END, 6, 0);
                    break;
                default: {
                    // inside P3
                    // create P8
                    int pid8 = fork();
                    switch (pid8) {
                    case -1:
                        perror("ERROR: Couldn't create a new child");
                        exit(1);
                        break;
                    case 0:
                        // print info about starting P8
                        info(BEGIN, 8, 0);

                        runProccess8();

                        // print info about ending P8
                        info(END, 8, 0);
                        break;
                    default:
                        // inside P3
                        // wait P4 to end
                        waitpid(pid4, NULL, 0);
                        // wait P6 to end
                        waitpid(pid6, NULL, 0);
                        // wait P8 to end
                        waitpid(pid8, NULL, 0);

                        // print info about ending P3
                        info(END, 3, 0);
                        break;
                    }
                    break;
                } }
                break;
            } }
            break;
        default: {
            // inside P1
            // create P7
            int pid7 = fork();
            switch (pid7) {
            case -1:
                perror("ERROR: Couldn't create a new child");
                exit(1);
                break;
            case 0:
                // print info about starting P7
                info(BEGIN, 7, 0);
                // print info about ending P7
                info(END, 7, 0);
                break;
            default:
                // inside P1
                // wait P2 to end
                waitpid(pid2, NULL, 0);
                // wait P3 to end
                waitpid(pid3, NULL, 0);
                // wait P7 to end
                waitpid(pid7, NULL, 0);

                // print info about ending P1
                info(END, 1, 0);
                break;
            }
            break;
        } }
        break;
    } }
    return 0;
}
