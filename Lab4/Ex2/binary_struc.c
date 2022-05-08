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
#include <errno.h>

typedef struct _OPERATION {
    int no1;
    int no2;
    char operator;
} OPERATION;

void write_operation(int fd, OPERATION *op) {
    lseek(fd, 0, SEEK_END);
    if (write(fd, op, sizeof(OPERATION)) < 0)
        perror("write failed");
}

int read_operation(int fd, OPERATION *op) {
    if (read(fd, op, sizeof(OPERATION)) <= 0)
        return 0;
    return 1;
}

void perform_operations(int fd, int out) {
    OPERATION op;
    while(read_operation(fd, &op) != 0) {
        if(op.operator == '+') {
            char output[255] = "";
            char number[10];
            //int length = 0;
            int res = op.no1 + op.no2;
            sprintf(number, "%d", op.no1);
            strcat(output, number);
            strcat(output, " + ");
            sprintf(number, "%d", op.no2);
            strcat(output, number);
            strcat(output, " = ");
            sprintf(number, "%d", res);
            strcat(output, number);
            strcat(output, "\n");
            printf("%s", output);
            write(out, output, strlen(output));
        }
    }
}

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("USAGE: binary_struc <bin_file> <text_file>\n");
        exit(1);
    }
    int bin = open(argv[1], O_RDONLY);
    if (bin < 0) {
        printf("Error while opening the binary file\n");
        exit(2);
    }
    int txt = open(argv[2], O_APPEND | O_RDWR);
    if (txt < 0) {
        printf("Error while opening the text file\n");
        exit(3);
    }

    perform_operations(bin, txt);
    close(txt);
    close(bin);
    return 0;
}