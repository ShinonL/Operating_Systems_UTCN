#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
	char a[20],b[20];
    char op[20];

    printf("Introduce an operation (a + b or a - b):\n");
    printf("> ");

	scanf("%s", a);
    scanf("%s", op);
    scanf("%s", b);

    int state;
	int pid = fork();

    switch (pid) {
        case -1:
            perror("Error creating new process");
		    exit(1);
            break;
        case 0:
            execl("./server", a, op, b, NULL);
        default:
            wait(&state);
            break;
    }


    // range 0-255
    state = WEXITSTATUS(state);

    int operand1;
    sscanf(a, "%d", &operand1);
    int operand2;
    sscanf(b, "%d", &operand2);
    if(op[0] == '-' && operand1 < operand2) {
        state -= 256;
    }

    printf("Result: %d\n", state);
}
