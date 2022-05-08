#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("USAGE: %s <file_name> <memory_size>\n", argv[0]);
        exit(1);
    }

    int memorySize;
    sscanf(argv[2], "%d", &memorySize);
    if (memorySize < 1) {
        printf("Memory size should be a positive integer\n");
        exit(1);
    }

    int shm_fd = shm_open(argv[1], O_CREAT | O_RDWR, 0777);
    if (shm_fd < 0) {
        perror("Couldn't aquire shm");
        exit(2);
    }

    ftruncate(shm_fd, memorySize * sizeof(char));

    char* shared_data = (char*)mmap(0, memorySize * sizeof(char), PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0);
    if (shared_data == MAP_FAILED) {
        perror("Mapping the shared memory failed");
        exit(2);
    }

    close(shm_fd);

    for (int i = 0; i < memorySize; i++) {
        shared_data[i] = 'A' + i % 26;
        printf("P1 wrote: %c\n", shared_data[i]);
        sleep(1);
    }

    munmap(shared_data, memorySize * sizeof(char));
    shared_data = NULL;
    shm_unlink(argv[1]);

    return 0;
}