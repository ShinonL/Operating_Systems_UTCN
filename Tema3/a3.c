#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#pragma pack(push, 1)
typedef struct _TOP_OF_HEADER {
    char magic[4];
    unsigned short headerSize;
    unsigned int version;
    unsigned char numberOfSections;
} TopOfHeader_T;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct _SECTION_HEADER {
    char name[10];
    unsigned char type;
    unsigned int sectionOffset;
    unsigned int sectionSize;
} SectionHeader_T;
#pragma pack(pop)


int requestPipe, responsePipe;
int shmFD;
char *shared_data, *mapped_file;
unsigned int memorySize, mappedFileSize;

void pingCommand() {
    char size = 4;
    write(responsePipe, &size, sizeof(char));
    write(responsePipe, "PING", 4 * sizeof(char));

    write(responsePipe, &size, sizeof(char));
    write(responsePipe, "PONG", 4 * sizeof(char));

    unsigned int code = 25380;
    write(responsePipe, &code, sizeof(unsigned int));
}

void createShmCommand() {
    read(requestPipe, &memorySize, sizeof(unsigned int));

    // common written message for both error and success
    char charSize = 10;
    write(responsePipe, &charSize, sizeof(char));
    write(responsePipe, "CREATE_SHM", 10 * sizeof(char));

    // create/open shared memory region
    shmFD = shm_open("/cJuRVmFI", O_CREAT | O_RDWR, 0664);
    if (shmFD < 0) {
        charSize = 5;
        write(responsePipe, &charSize, sizeof(char));
        write(responsePipe, "ERROR", 5 * sizeof(char));
        return;
    }

    // truncate to the given size
    ftruncate(shmFD, memorySize * sizeof(char));
    // map the memory
    shared_data = (char *)mmap(0, memorySize * sizeof(char), PROT_READ | PROT_WRITE, MAP_SHARED, shmFD, 0);
    if (shared_data == MAP_FAILED) {
        charSize = 5;
        write(responsePipe, &charSize, sizeof(char));
        write(responsePipe, "ERROR", 5 * sizeof(char));
        return;
    }

    charSize = 7;
    write(responsePipe, &charSize, sizeof(char));
    write(responsePipe, "SUCCESS", 7 * sizeof(char));
}

void wrtieToShmCommand() {
    unsigned int offset, value;
    read(requestPipe, &offset, sizeof(unsigned int));
    read(requestPipe, &value, sizeof(unsigned int));

    // common written message for both error and success
    char charSize = 12;
    write(responsePipe, &charSize, sizeof(char));
    write(responsePipe, "WRITE_TO_SHM", 12 * sizeof(char));

    if (offset + sizeof(unsigned int) >= memorySize) {
        charSize = 5;
        write(responsePipe, &charSize, sizeof(char));
        write(responsePipe, "ERROR", 5 * sizeof(char));
        return;
    }

    unsigned int* memoryPointer = (unsigned int *)(shared_data + offset);
    *memoryPointer = value;

    charSize = 7;
    write(responsePipe, &charSize, sizeof(char));
    write(responsePipe, "SUCCESS", 7 * sizeof(char));
}

void mapFileCommand() {
    // read file name size
    char charSize;
    read(requestPipe, &charSize, sizeof(char));
    int fileNameSize = charSize;

    // read the actual name of the file
    char* fileName = (char *)malloc((fileNameSize + 1)* sizeof(char));
    read(requestPipe, fileName, fileNameSize * sizeof(char));
    fileName[fileNameSize] = '\0';

    // common written message for both error and success
    charSize = 8;
    write(responsePipe, &charSize, sizeof(char));
    write(responsePipe, "MAP_FILE", 8 * sizeof(char));

    // open file
    int fileFD = open(fileName, O_RDONLY);
    if (fileFD < 0) {
        charSize = 5;
        write(responsePipe, &charSize, sizeof(char));
        write(responsePipe, "ERROR", 5 * sizeof(char));
        return;
    }

    struct stat fileStat;
    lstat(fileName, &fileStat);
    mappedFileSize = fileStat.st_size;

    if (mappedFileSize <= 0) {
        charSize = 5;
        write(responsePipe, &charSize, sizeof(char));
        write(responsePipe, "ERROR", 5 * sizeof(char));
        return;
    }

    mapped_file = (char *)mmap(0, mappedFileSize * sizeof(char), PROT_READ, MAP_SHARED, fileFD, 0);
    if (mapped_file == MAP_FAILED) {
        charSize = 5;
        write(responsePipe, &charSize, sizeof(char));
        write(responsePipe, "ERROR", 5 * sizeof(char));

        free(fileName);
        close(fileFD);

        return;
    }

    free(fileName);
    close(fileFD);

    charSize = 7;
    write(responsePipe, &charSize, sizeof(char));
    write(responsePipe, "SUCCESS", 7 * sizeof(char));
}

void readFromMappedFIle() {
    unsigned int offset, numberOfBytes;
    read(requestPipe, &offset, sizeof(unsigned int));
    read(requestPipe, &numberOfBytes, sizeof(unsigned int));

    // common written message for both error and success
    char charSize = 21;
    write(responsePipe, &charSize, sizeof(char));
    write(responsePipe, "READ_FROM_FILE_OFFSET", 21 * sizeof(char));

    if (shared_data == MAP_FAILED || mapped_file == MAP_FAILED || offset + numberOfBytes >= mappedFileSize) {
        charSize = 5;
        write(responsePipe, &charSize, sizeof(char));
        write(responsePipe, "ERROR", 5 * sizeof(char));
        return;
    }

    int index = 0;
    for (int i = offset; i < offset + numberOfBytes; i++, index++) {
        shared_data[index] = mapped_file[i];
    }

    charSize = 7;
    write(responsePipe, &charSize, sizeof(char));
    write(responsePipe, "SUCCESS", 7 * sizeof(char));
}

void readFromFileSection() {
    unsigned int sectionNumber, offset, numberOfBytes;
    read(requestPipe, &sectionNumber, sizeof(unsigned int));
    read(requestPipe, &offset, sizeof(unsigned int));
    read(requestPipe, &numberOfBytes, sizeof(unsigned int));

    // common written message for both error and success
    char charSize = 22;
    write(responsePipe, &charSize, sizeof(char));
    write(responsePipe, "READ_FROM_FILE_SECTION", 22 * sizeof(char));

    TopOfHeader_T* topOfHeader = (TopOfHeader_T *)mapped_file;
    if ((int)topOfHeader->numberOfSections < sectionNumber) {
        charSize = 5;
        write(responsePipe, &charSize, sizeof(char));
        write(responsePipe, "ERROR", 5 * sizeof(char));
        return;
    }

    SectionHeader_T *sectionHeader = (SectionHeader_T *)(mapped_file + sizeof(TopOfHeader_T) + (sectionNumber - 1) * sizeof(SectionHeader_T));
    if (sectionHeader->sectionOffset + offset + numberOfBytes >= memorySize || offset + numberOfBytes > sectionHeader->sectionSize) {
        charSize = 5;
        write(responsePipe, &charSize, sizeof(char));
        write(responsePipe, "ERROR", 5 * sizeof(char));
        return;
    }

    for (int i = 0; i < numberOfBytes; i++) {
        shared_data[i] = mapped_file[sectionHeader->sectionOffset + offset + i];
    }

    charSize = 7;
    write(responsePipe, &charSize, sizeof(char));
    write(responsePipe, "SUCCESS", 7 * sizeof(char));
}

void readFromLogicalSpace() {
    unsigned int logicalOffset, numberOfBytes;
    read(requestPipe, &logicalOffset, sizeof(unsigned int));
    read(requestPipe, &numberOfBytes, sizeof(unsigned int));

    // common written message for both error and success
    char charSize = 30;
    write(responsePipe, &charSize, sizeof(char));
    write(responsePipe, "READ_FROM_LOGICAL_SPACE_OFFSET", 30 * sizeof(char));

    // TopOfHeader_T* topOfHeader = (TopOfHeader_T *)mapped_file;
    SectionHeader_T *sectionHeader = (SectionHeader_T *)(mapped_file + sizeof(TopOfHeader_T));
    int sectionNumber = 0;
    while(logicalOffset > sectionHeader->sectionSize) {
        int fillBuffer = (sectionHeader->sectionSize % 1024 == 0) ? 0 : 1;
        logicalOffset -= 1024 * (sectionHeader->sectionSize / 1024 + fillBuffer);

        sectionNumber++;
        sectionHeader = (SectionHeader_T *)(mapped_file + sizeof(TopOfHeader_T) + sizeof(SectionHeader_T) * sectionNumber);
    }

    if (logicalOffset < 0 || logicalOffset + numberOfBytes > sectionHeader->sectionSize) {
        charSize = 5;
        write(responsePipe, &charSize, sizeof(char));
        write(responsePipe, "ERROR", 5 * sizeof(char));
        return;
    }

    for (int i = 0; i < numberOfBytes; i++) {
        shared_data[i] = mapped_file[sectionHeader->sectionOffset + logicalOffset + i];
    }

    charSize = 7;
    write(responsePipe, &charSize, sizeof(char));
    write(responsePipe, "SUCCESS", 7 * sizeof(char));
}

int main(int argc, char** argv) {
    char* responsePath = "./RESP_PIPE_25380";
    char* requestPath = "./REQ_PIPE_25380";

    // create response pipe
    if (mkfifo(responsePath, 0777) < 0) {
        printf("ERROR\ncannot create  the response pipe\n");
        exit(2);
    }

    // open for reading the request pipe
    requestPipe = open(requestPath, O_RDONLY);
    if (requestPath < 0) {
        printf("ERROR\ncannot open the request pipe\n");
        exit(2);
    }
    
    // open for witing the response pipe
    responsePipe = open(responsePath, O_WRONLY);

    // announce the connection
    char charSize = 7;
	write(responsePipe, &charSize, sizeof(char));
	write(responsePipe, "CONNECT", sizeof(char) * 7);

    printf("SUCCESS\n");

    int size;
    // charSize = '\0';
    char* command = (char *)malloc(40 * sizeof(char));

    for(;;) {
        read(requestPipe, &charSize, sizeof(char));
        size = charSize;
        read(requestPipe, command, size * sizeof(char));
        command[size] = '\0';


        if (strcmp(command, "PING") == 0) {
            pingCommand();
            continue;
        }
        if (strcmp(command, "CREATE_SHM") == 0) {
            createShmCommand();
            continue;
        }
        if (strcmp(command, "WRITE_TO_SHM") == 0) {
            wrtieToShmCommand();
            continue;
        }
        if (strcmp(command, "MAP_FILE") == 0) {
            mapFileCommand();
            continue;
        }
        if (strcmp(command, "READ_FROM_FILE_OFFSET") == 0) {
            readFromMappedFIle();
            continue;
        }
        if (strcmp(command, "READ_FROM_FILE_SECTION") == 0) {
            readFromFileSection();
            continue;
        }
        if (strcmp(command,  "READ_FROM_LOGICAL_SPACE_OFFSET") == 0) {
            readFromLogicalSpace();
            continue;
        }
        if (strcmp(command, "EXIT") == 0) 
            break;
    }

    free(command);
    return 0;
}