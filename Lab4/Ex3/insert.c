// C program headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// open file headers
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// read file headers
#include <unistd.h>

#define MAX_SIZE 10000

void insertInFile(int destination, int position, char* stringToInsert) {
    int seek = lseek(destination, position, SEEK_SET);
    if (seek < 0) {
        printf("Error while positioning in the file.\n");
        exit(4);
    }
    int written = write(destination, stringToInsert, strlen(stringToInsert));
    if (written < 0) {
        printf("Error while writing in the file.\n");
        exit(5);
    }
}

void shiftToRight(int destination, int position, char *shiftedString) {
    int length = 0;
    int seek = lseek(destination, position, SEEK_SET);
    if (seek < 0) {
        printf("Error while positioning in the file.\n");
        exit(4);
    }
    char character;
    while(read(destination, &character, 1) > 0) {
        if(length + 1 == MAX_SIZE) {
            printf("The file id too big. Please make it smalles.\n");
            exit(6);
        }
        shiftedString[length++] = character;
    }
}

int main(int argc, char** argv) {
    if(argc != 4) {
        printf("USAGE: insert <file_name> <position> \"<string>\"\n");
        exit(1);
    }
    int destination = open(argv[1], O_RDWR);
    if(destination < 0) {
        printf("Error while opening the destination file.\n");
        exit(2);
    }
    int position = 0;
    for (int i = 0; i < strlen(argv[2]); i++) {
        position = position*10 + argv[2][i] - '0';
    }
    if (position < 0) {
        printf("Position must be greater than or equal to 0\n");
        exit(3);
    }

    char shiftedString[MAX_SIZE];
    shiftToRight(destination, position, shiftedString);
    insertInFile(destination, position, argv[3]);
    insertInFile(destination, position + strlen(argv[3]), shiftedString);
    return 0;
}