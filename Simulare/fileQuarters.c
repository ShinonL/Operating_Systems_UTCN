#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <dirent.h>

int main(int argc, char** argv) {
    int fd = open("./TEST", O_RDWR);
    if (fd < 0) {
        printf("ERROR\ncould not open file\n");
        exit(1);
    }

    struct stat fileStat;
    lstat("./TEST", &fileStat);
    printf("File size in bytes %ld\n", fileStat.st_size);

    long int fileSize = (fileStat.st_size / 16) * 16;
    ftruncate(fd, fileSize);

    lseek(fd, fileSize/4, SEEK_SET);

    char* readData = (char *)malloc(fileSize/4 * sizeof(char));
    read(fd, readData, fileSize/4);
    printf("%s\n", readData);

    char* mapped_file = (char *)mmap(0, fileSize/4 * sizeof(char), PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, fileSize/4*3);

    int sum = 0;
    for (int i = 0; i < fileSize/4; i+=4) {
        sum += (int)mapped_file[i];
    }

    printf("Sum %d\n", sum);
    return 0;
}