#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include <string.h>

#include <sys/types.h>
#include <unistd.h>

#include <pthread.h>

void* executeThread(void* parameters) {
    long long left = *(long long *)(parameters + sizeof(long long));
    long long right = *(long long *)(parameters + 2*sizeof(long long));
    long long numberM = *(long long *)(parameters +  3*sizeof(long long));
    printf("Parameters left: %lld  right: %lld\n", left, right);
    for(long long number = left + 1; number <= right && number <= numberM; number++) {
        *(long long *)(parameters) *= number;
        *(long long *)(parameters) %= LLONG_MAX-1;
    }

    pthread_exit(NULL);
}
int main(int argc, char** argv) {
    if (argc != 3) {
        printf("USAGE: ./ex3 <number_of_threads> <number_M>\n");
        exit(1);
    }
    int numberOfThreads, numberM;
    sscanf(argv[1], "%d", &numberOfThreads);
    sscanf(argv[2], "%d", &numberM);

    if (numberM <= 1) {
        printf("Please choose a number greater than 1\n");
        exit(1);
    }
    
    printf("[Thread MAIN] Started TID %ld\n", (long int)pthread_self());

    pthread_t* threads = (pthread_t *)malloc(numberOfThreads * sizeof(pthread_t));
    long long* parameters = (long long *)malloc(4 * sizeof(long long));
    parameters[0] = 1;
    for(int i = 0; i < numberOfThreads; i++) {
        parameters[1] = (numberM / numberOfThreads) * i;
        parameters[2] = (numberM / numberOfThreads) * (i + 1);
        parameters[3] = numberM;
        if (i == numberOfThreads - 1)
            parameters[2] += numberM % numberOfThreads;

        printf("Parameters left: %lld  right: %lld\n", parameters[1], parameters[2]);
        
        if (pthread_create(&threads[i], NULL, executeThread, parameters) != 0) {
            perror("Error creating a new thread");
            exit(1);
        }

        pthread_join(threads[i], NULL);
    }

    printf("Solution %lld\n", parameters[0]);

    printf("[Thread MAIN] Terminated TID %ld\n", (long int)pthread_self());
    return 0;
}