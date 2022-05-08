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
               *line_length = length;
               line[length] = 0;
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

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("USAGE: %s source_file line_number\n", argv[0]);
        exit(1);
    }

    int source, lineNumber;
    source = open(argv[1], O_RDONLY);
    if (source < 0) {
        printf("Error while opening the source file\n");
        exit(2);
    }
    lineNumber = 0;
    for (int i = 0; i < strlen(argv[2]); i++) {
        lineNumber = lineNumber*10 + argv[2][i] - '0';
    }
    if (lineNumber < 0) {
        printf("Line number must be greater than or equal to 0\n");
        exit(3);
    }

    char line[255];
    int length;
    int result = get_line(source, line, lineNumber, 255, &length);
    printf("Return result: %d\n", result);
    if(result == 0)
        printf("Line: %s\nLenght: %d\n", line, length);
    else printf("Reading failed.\n");
    return 0;
}