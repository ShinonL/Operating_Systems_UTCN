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

int get_line(int fd, char *line, int line_no, int max_length, int *line_length) {
    char letter;
    int lineCount = 0, length = 0;

    while(read(fd, &letter, 1) > 0) {
        if(letter == '\n' || length > max_length - 1) {
            if(lineCount == line_no) {
               line[length++] = '\n';
               line[length] = '0';
               *line_length = length;
                return 0;
            } else {
                while(letter != '\n') {
                    read(fd, &letter, 1);
                }
                strcpy(line, "");
                length = 0;
                lineCount++;
            }
        } else {
            line[length] = letter;
            length++;
        }
    }
    return -1;
}

int getNumberOfLines(int fd) {
    int lineCount = -1;
    char letter;
    while(read(fd, &letter, 1) > 0) {
        if(letter == '\n')
            lineCount++;
    }
    return lineCount;
}

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("USAGE: %s source_file destination_file\n", argv[0]);
        exit(1);
    }

    int source, destination;
    source = open(argv[1], O_RDONLY);
    if (source < 0) {
        printf("Error while opening the source file\n");
        exit(2);
    }
    destination = open(argv[2], O_APPEND | O_WRONLY);
    if (destination < 0) {
        printf("Error while opening the destination file\n");
        exit(3);
    }


    int numberOfLines = getNumberOfLines(source);
    while(numberOfLines >= 0) {
        lseek(source, 0, SEEK_SET);
        char line[255];
        int length = 0;
        int result = get_line(source, line, numberOfLines, 255, &length);
        if(result == 0)
            write(destination, line, length);
        numberOfLines--;
    }
    return 0;
}
