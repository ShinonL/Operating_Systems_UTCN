#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t lock, maximumLock;
pthread_cond_t maximumCond;
pthread_mutex_t* passingLocks;
pthread_cond_t* passingCond;

typedef struct {
    int car_id;
    int car_direction;
} car_args;

int MAX;
int passing_direction, numberOfCarsOnBridge;

void* car(void *arg) {
    car_args car = *(car_args *)arg;

    if (pthread_mutex_lock(&maximumLock) != 0) {
        perror("Couldn't take the lock");
        exit(3);
    }
    while (numberOfCarsOnBridge >= MAX) {
        if (pthread_cond_wait(&maximumCond, &maximumLock) != 0) {
            perror("Couldn't wait for the condition");
            exit(3);
        }
    }
    if (pthread_mutex_unlock(&maximumLock) != 0) {
        perror("Couldn't take the lock");
        exit(3);
    }

    printf("%d\n", car.car_id);

    if (pthread_mutex_lock(&passingLocks[car.car_direction]) != 0) {
        perror("Couldn't take the lock");
        exit(3);
    }
    while (car.car_direction != passing_direction) {
        if (pthread_cond_wait(&passingCond[passing_direction], &passingLocks[car.car_direction]) != 0) {
            perror("Couldn't wait for the condition");
            exit(3);
        }
    }
    if (pthread_mutex_unlock(&passingLocks[car.car_direction]) != 0) {
        perror("Couldn't take the lock");
        exit(3);
    }

    if (pthread_mutex_lock(&lock) != 0) {
        perror("Couldn't take the lock");
        exit(3);
    }
    numberOfCarsOnBridge++;
    if (pthread_mutex_unlock(&lock) != 0) {
        perror("Couldn't take the lock");
        exit(3);
    }

    // enter_bridge(car.car_id, car.car_direction);
    printf("Car %d entered the bridge on direction %d\n", car.car_id, car.car_direction);

    // pass_bridge(car.car_id, car.car_direction);
    printf("Car %d is passing the bridge on direction %d\n", car.car_id, car.car_direction);
    
    // exit_bridge(car.car_id, car.car_direction);
    printf("Car %d exited the bridge on direction %d\n", car.car_id, car.car_direction);

    if (pthread_mutex_lock(&lock) != 0) {
        perror("Couldn't take the lock");
        exit(3);
    }
    numberOfCarsOnBridge--;
    if (numberOfCarsOnBridge == MAX - 1)
        pthread_cond_signal(&maximumCond);
    if (pthread_mutex_unlock(&lock) != 0) {
        perror("Couldn't take the lock");
        exit(3);
    }

    return NULL;
}
void* trafic_controller(void* arg) {
    while (1) {
        // printf("%d\n", passing_direction);
        sleep(5);

        if (pthread_mutex_lock(&lock) != 0) {
            perror("Couldn't take the lock");
            exit(3);
        }
        passing_direction = (passing_direction + 1) % 2;
        if (pthread_mutex_unlock(&lock) != 0) {
            perror("Couldn't take the lock");
            exit(3);
        }
        pthread_cond_broadcast(&passingCond[passing_direction]);
        // change_passing_direction();
    }
   return NULL;
}

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("USAGE: %s <number_of_cars> <maximum_number_of_cars>\n", argv[0]);
        exit(1);
    }

    int numberOfCars;
    sscanf(argv[1], "%d", &numberOfCars);
    if (numberOfCars <= 0) {
        printf("The number of cars should be greater than 0\n");
        exit(1);
    }

    sscanf(argv[2], "%d", &MAX);
    if (MAX <= 0) {
        printf("The max number of cars on the bridge should be greater than 0\n");
        exit(1);
    }

    if (pthread_mutex_init(&maximumLock, NULL) != 0) {
        perror("Couldn't initialize the lock");
        exit(2);
    }
    if (pthread_cond_init(&maximumCond, NULL) != 0) {
        perror("Couldn't create the condition variable");
        exit(2);
    }

    passingLocks = (pthread_mutex_t *)malloc(2 * sizeof(pthread_mutex_t));
    passingCond = (pthread_cond_t *)malloc(2 * sizeof(pthread_cond_t));
    for (int i = 0; i < 2; i++) {
        if (pthread_mutex_init(&passingLocks[i], NULL) != 0) {
            perror("Couldn't initialize the lock");
            exit(2);
        }
        if (pthread_cond_init(&passingCond[i], NULL) != 0) {
            perror("Couldn't create the condition variable");
            exit(2);
        }
    }

    if (pthread_mutex_init(&lock, NULL) != 0) {
        perror("Couldn't initialize the lock");
        exit(2);
    }

    pthread_t* threads = (pthread_t *)malloc(numberOfCars * sizeof(pthread_t));
    car_args* cars = (car_args *)malloc(numberOfCars * sizeof(car_args));
    for (int i = 0; i < numberOfCars; i++) {
        cars[i].car_id = i + 1;
        cars[i].car_direction = rand() % 2;

        if (pthread_create(&threads[i], NULL, &car, &cars[i])) {
            perror("Couldn't create the threads");
            exit(3);
        }
    }

    pthread_t controller;
    if (pthread_create(&controller, NULL, &trafic_controller, NULL) != 0) {
        perror("Couldn't create the thread");
        exit(3);
    }

    for (int i = 0; i < numberOfCars; i++) 
        pthread_join(threads[i], NULL);
    pthread_join(controller, NULL);

    if (pthread_mutex_destroy(&lock) != 0) {
        perror("Couldn't destroy the lock");
        exit(4);
    }
    if (pthread_mutex_destroy(&maximumLock) != 0) {
        perror("Couldn't destroy the lock");
        exit(4);
    }
}