#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <string.h>

#include <sys/types.h>
#include <unistd.h>

#include <pthread.h>

int sleepTime;

void* executeControlThread(void *stop) {
    printf("[THREAD Control] Start\n");
    while(scanf("%c", (char *)stop)) {
        break;
    }
    printf("[THREAD Control] Stop\n");
    return NULL;
}
void* executeClientThread(void* ID) {
    printf("[Client thread %d] Starting ...\n", *(int *)ID);
    srand(time(NULL));
    int r = rand();
    sleep(r % 10);
    printf("[Client thread %d] Ending ...\n", *(int *)ID);
    return NULL;
}
void* executeConnectionThread(void *stop) {
    printf("[THREAD Connection] Start\n");
    int i = 1;
    while(*(char *)stop != 'x') {
        pthread_t clientThread;
        if (pthread_create(&clientThread, NULL, executeClientThread, &i) != 0) {
            perror("Error creating the control thread");
            exit(1);
        }
        sleep(10);
    }
    printf("[THREAD Connection] Stop\n");
    return NULL;
}

int main(int argc, char** argv) {
    if (argc != 1) {
        printf("USAGE: ./ex3\n");
        exit(1);
    }

    //sleepTime = atoi(argv[1]);
    //if (sleepTime < 1) {
    //    printf("Please choose a time greater than 0\n");
    //    exit(1);
    //}
    
    printf("[Thread MAIN] Started TID %ld\n", (long int)pthread_self());

    pthread_t controlThread, connectionThread;
    //int* controlState, connectionState;
    char stop;

    if (pthread_create(&controlThread, NULL, executeControlThread, &stop) != 0) {
        perror("Error creating the control thread");
        exit(1);
    }

    if (pthread_create(&connectionThread, NULL, executeConnectionThread, &stop) != 0) {
        perror("Error creating the control thread");
        exit(1);
    }

    pthread_join(controlThread, NULL);
    pthread_join(connectionThread, NULL);

    printf("[THREAD Main] Stop\n");
    return 0;
}