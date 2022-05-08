#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>


int main(int argc, char** argv) {
    int pid = fork();
    switch (pid) {
        case -1:
            perror("Cannot create a new child");
            exit(1);
        case 0:
            printf("P1: %d", getpid());
            // child 
            //... does what it is requested ...
            break;
        default:
            printf("        PO: %d\n", getpid());

            // asteptam sa se termine cu copilul P1
            waitpid(pid, NULL, 0);

            // cream P2
            int pid2 = fork();
            switch (pid2) {
                case -1:
                    perror("Cannot create a new child");
                    exit(1);
                case 0:
                    // in P2
                    printf("        P2: %d\n", getpid());

                    // cream P3
                    int pid3 = fork();
                    switch (pid3) {
                        case -1:
                            perror("Cannot create a new child");
                            exit(1);
                        case 0:
                            // in P3
                            printf("         P3: %d", getpid());
                            break;
                        default:
                            // asteptam terminarea P3
                            waitpid(pid3, NULL, 0);

                            // cream P4
                            int pid4 = fork();
                            switch (pid4) {
                                case -1:
                                    perror("Cannot create a new child");
                                    exit(1);
                                case 0:
                                    printf("    P4: %d\n", getpid());
                                    break;
                                default:
                                    // aici suntem inapoi in P2
                                    // asteptam P4
                                    waitpid(pid4, NULL, 0);
                            }
                    }
                    break;
                default:
                    // aici se intoarce inapoi in P1
                    // asteptam terminarea P2
                    waitpid(pid2, NULL, 0);
            }
    } 
}
