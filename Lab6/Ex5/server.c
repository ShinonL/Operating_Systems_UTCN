#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char **argv) {
    int operand1;
    sscanf(argv[0], "%d", &operand1);
    char op = argv[1][0];
    int operand2;
    sscanf(argv[2], "%d", &operand2);

    if(op == '+')
        exit(operand1 + operand2);
    if(op == '-')
        exit(operand1 - operand2);
}
