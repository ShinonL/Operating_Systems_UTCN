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

    char a[20],b[20];
    char op[20];

    for(;;) {
        printf("Also run the server.\nIntroduce an operation (a + b or a - b):\n");
        printf("> ");

        scanf("%s", a);
        if (strcmp(a, "x") == 0 || strcmp(a, "X") == 0) {
            int fdWrite = open(path1, O_WRONLY);
            write(fdWrite, &a[0], sizeof(a[0]));
            exit(0);
        }

        scanf("%s", op);
        scanf("%s", b);

        int leftOperand, rightOperand;
        char operator;

        sscanf(a, "%d", &leftOperand);
        sscanf(op, "%c", &operator);
        sscanf(b, "%d", &rightOperand);

        int fdWrite = open(path1, O_WRONLY);
        write(fdWrite, &operator, sizeof(operator));
        write(fdWrite, &leftOperand, sizeof(leftOperand));
        write(fdWrite, &rightOperand, sizeof(rightOperand));
        close(fdWrite);

        int fdRead = open(path2, O_RDONLY);
        int result;
        read(fdRead, &result, sizeof(result));
        printf("Server Result: %d\n", result);
    }
    return 0;
}