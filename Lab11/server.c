#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    char *path1 = "./ClientToServerPipe";
    char *path2 = "./ServerToClientPipe";
    mkfifo(path1, 0666);
    mkfifo(path2, 0666);

    for(;;) {
        int fdRead = open(path1, O_RDONLY);

        int leftOperand, rightOperand;
        char operator;

        read(fdRead, &operator, sizeof(operator));

        int fdWrite = open(path2, O_WRONLY);
        int result;
        switch (operator) {
        case '+':
            read(fdRead, &leftOperand, sizeof(leftOperand));
            read(fdRead, &rightOperand, sizeof(rightOperand));

            printf("Operation received: %d %c %d\n", leftOperand, operator, rightOperand);

            result = leftOperand + rightOperand;
            break;
        case '-':
            read(fdRead, &leftOperand, sizeof(leftOperand));
            read(fdRead, &rightOperand, sizeof(rightOperand));

            printf("Operation received: %d %c %d\n", leftOperand, operator, rightOperand);

            result = leftOperand - rightOperand;
            break;
        case 'x':
        case 'X':
            exit(0);
        default:
            result = 0;
            break;
        }
        close(fdRead);

        printf("Operation result sent to client: %d\n", result);
        write(fdWrite, &result, sizeof(result));
        close(fdWrite);
    }
    return 0;
}