#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
	int childPid; 
		
	printf("[Parent] PID=%d ParentPID=%d\n", getpid(), getppid());
	
	// Create a new child	
	childPid = fork();
	
	// Check if successfully created the child
	if (childPid < 0) { // if not, terminate the parent too
		perror("Error creating new process");
		exit(1);
	}
		
	// Here we separare between the code executed by the parent and child
	if (childPid > 0) { //parent
		printf("[Parent] ChildPID=%d\n", childPid);
		
		// avem 3 argumente si ultimul nu e & sau avem doar 2 argumente
		if((argc == 3 && strcmp(argv[2], "&") != 0) || argc == 2)
			waitpid(childPid, NULL, 0);
		
		printf("[Parent] Terminate the execution\n");
	} else {	    // child
		printf("[Child] PID=%d ParentPID=%d\n", getpid(), getppid());
		printf("[Child] Load and execute a new code. The actual one is overwritten and lost. There is no return from a successfull exec!!!\n");
		
		// avem 3 argumente si ultimul e & sau avem doar 2 argumente
        if (argc == 2 || (argc == 3 && strcmp(argv[2], "&") == 0)) {
            execlp(argv[1], argv[1], NULL);
		
		// avem 4 argumente si ultimul e & sau avem doar 3 argumente (cazul cu & e exclus in if-ul de sus)
        } else if (argc == 3 || (argc == 4 && strcmp(argv[3], "&") == 0)) {
            execlp(argv[1], argv[1], argv[2], NULL);
        } else {
            printf("USAGE: ./ex4 <command> [parameters] [\"&\"]\n");
            exit(2);
        }
		
		// Putting code here makes sense only when execlp does not execute successfully
		perror("Error executing exec");
		printf("[Child] Terminate the execution\n");
		exit(1);
	}
    return 0;
}

