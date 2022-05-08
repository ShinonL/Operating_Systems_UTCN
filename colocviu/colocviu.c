#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>

// Write a program that counts the words in a file using mmap and threads for better performance.
void P(sem_t *semaphore) {
    sem_wait(semaphore);
}
void V(sem_t *semaphore) {
    sem_post(semaphore);
}

sem_t sem;

int numberOfThreads, fd;
char *mapped;
int size, countW;

void* count(void* args) {
    int left = *(int *)args;
    int right = size/numberOfThreads * (left + 1);

    for (int i = left; i < right; i++) {
        if ((mapped[i] == ' ' || mapped[i] == '\n') && mapped[i-1] != ' ' && mapped[i-1] != '\n') 
        {
            P(&sem);
            countW++;
            V(&sem);
        }
    }

    return NULL;
    
}

int main(int argc, char **argv)
{
    char *p  = (char*)malloc(137);
    if(p==NULL)
    exit(1);
    for(int i = 0; i<137;i++)
    p[i] = i;
    free(p);
    return 0;
}