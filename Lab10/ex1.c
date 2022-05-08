#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

int no_of_persons;
int no_of_persons_inside = 0;

sem_t lightON, personArrived, personLeft, mutex;

void P(sem_t *semaphore) {
    sem_wait(semaphore);
}
void V(sem_t *semaphore) {
    sem_post(semaphore);
}

// Recommandation - write firstly the solution for one person  
void* person(void* arg) {   
    int id = *(int*) arg;
    printf("Person %d has just arrived in front of the room's door\n", id);

    // TODO: if first person, i.e. no other person already in the room), 
    // 1. announce the controller a person has arrived, allowing it to switch the light on 
    // 2. wait for the light to be on
    P(&mutex);
    no_of_persons_inside++;
    if (no_of_persons_inside == 1) {
        V(&personArrived);
    }
    V(&mutex);

    P(&lightON);
    V(&lightON);

    printf("Person %d is in the room. No of persons in room = %d\n", id, no_of_persons_inside);
    usleep(rand() % 1001);
    printf("Person %d is leaving the room. No of persons in room = %d\n", id,  no_of_persons_inside);

    // TODO: if the last person exiting the room (i.e. room becomes empty)
    //  signal the controller it can swicth the ligh toff
    P(&mutex);
    no_of_persons_inside--;
    if (no_of_persons_inside == 0)
        V(&personLeft);
    no_of_persons--;
    V(&mutex);
    return NULL;
}

void* light_controller(void* arg) {
    while(no_of_persons > 0) {
        // TODO: wait for person(s) to arrive in front of room's door
        P(&personArrived);

        printf("Light is ON\n");
        
        // TODO: allow the person(s) to enter the room
        V(&lightON);

        // TODO: wait for room to become empty (last person to leave)
        P(&personLeft);

        printf("Light is OFF\n");

        P(&lightON);
    }
    return NULL;
}

int main(int argc, char **argv) {
    pthread_t *person_th;
    int       *person_arg;
    pthread_t controller_th;

    if (argc != 2) {
        printf("Error. Usage: %s <no_of_persons>\n", argv[0]);
        exit(1);
    }

    if (sem_init(&personLeft, 0, 0) < 0) {
        perror("ERROR: Couldn't create the semaphore set");
        exit(2);
    }
    if (sem_init(&personArrived, 0, 0) < 0) {
        perror("ERROR: Couldn't create the semaphore set");
        exit(2);
    }
    if (sem_init(&lightON, 0, 0) < 0) {
        perror("ERROR: Couldn't create the semaphore set");
        exit(2);
    }
    if (sem_init(&mutex, 0, 1) < 0) {
        perror("ERROR: Couldn't create the semaphore set");
        exit(2);
    }

    sscanf(argv[1], "%d", &no_of_persons); 
    srand(time(NULL));

    person_th = (pthread_t*) malloc(no_of_persons * sizeof(pthread_t));
    if (person_th == NULL) {
        perror("Cannot allocate memory for threads");
        exit(2);
    }

    person_arg = (int*) malloc(no_of_persons * sizeof(int));
    if (person_arg == NULL) {
        perror("Cannot allocate memory for person ids");
        exit(2);
    }

    if (pthread_create(&controller_th, NULL, light_controller, NULL) != 0) {
            perror("Cannot create controller threads");
            exit(3);
    }

    for (int i = 0; i < no_of_persons; i++) {
        person_arg[i] = i;
        if (pthread_create(&person_th[i], NULL, person, &person_arg[i]) != 0) {
            perror("Cannot create person threads");
            exit(3);
        }
    }

    for (int i = 0; i < no_of_persons; i++) {
        if (pthread_join(person_th[i], NULL) != 0) {
            perror("Cannot join person threads");
            exit(4);
        }
    }

    if (pthread_join(controller_th, NULL) != 0) {
        perror("Cannot join controller thread");
        exit(4);
    }

    sem_destroy(&lightON);
    sem_destroy(&personArrived);
    sem_destroy(&personLeft);
    sem_destroy(&mutex);

    fflush(stdout);
    return 0;
}