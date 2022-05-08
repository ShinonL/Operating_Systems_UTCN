#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define STATUS_SUCCESS          0
#define STATUS_BAD_NO_OF_ARGS   1
#define STATUS_ERR_BAD_NUMBER   2
#define STATUS_ERR_NOT_PRIME    3
            
                               
/* String containing name the program is called with.  */
const char *program_name;

int check_if_no(char *string, int *no)
{
        if (sscanf(string, "%d", no) < 1) {
                return  STATUS_ERR_BAD_NUMBER;
        }
        
        return STATUS_SUCCESS;
}

int check_if_prime(int no)
{
        int i;
        for (i = 2; i <= sqrt(no); i++)
                if (no % i == 0)
                        return STATUS_ERR_NOT_PRIME; 
        
        return STATUS_SUCCESS;
}


int main(int argc, char **argv)
{
	int no;		// used to store the number given as parameter
        int result;     // used to store the result of called functions

	program_name = argv[0];

	// Check the if command line parameters specified
	if (argc != 2) { // the expected parameter not specified
		fprintf(stderr, "Usage: %s <no>\n", argv[0]);
		exit(STATUS_BAD_NO_OF_ARGS);
	}
	
	// Check if given parameter is a number
	if ( (result = check_if_no(argv[1], &no)) != STATUS_SUCCESS) {
	        printf("NOT A NUMBER!\n");
		exit(result);
	}
	
	
	// Check if given number is prime
	if ( (result = check_if_prime(no)) != STATUS_SUCCESS) {
	        printf("NOT PRIME\n");
		exit(result);
	}
	
	printf("PRIME\n");
	
	return STATUS_SUCCESS;
}
