/*
Sa se citeaca un sir de numere de la tastatura. Sa se calculeze suma numerelor prime din sir.
Exemplu: pentru sirul [2,62,7,4,45,1], programul va afisa valoarea 10.

Read an integer array from  the keyboard. Compute the sum of the prime numbers from the array.
Example: for the array [2,62,7,4,45,1] the program will display the value 10.


*/
#include <stdio.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0

int isPrime(int number) {
	int i;
    if (number == 1 || number == 0) {
        return FALSE;
    }

	for(i=2; i*i <= number; i++){
		if(number % i == 0){
			return FALSE;
		}
	}
	return TRUE;
}

int computeSum(int *array, int size)
{
	int i, sum = 0;


	for(i = 0; i < size; i++){
		if(isPrime(array[i]) == TRUE){
			sum += array[i];
            //printf("%d\n", array[i]);
		}
	}

 	return sum;
}


int readArray(int *size, int *array)
{
	int i, sz;

	printf("Size: \n");
	if (scanf("%d", &sz) != 1){ 
		printf("Error reading size!\n");
		return FALSE;
	}

	printf("Array:\n");
	for(i=0;i<sz;i++){
		if(scanf("%d", &array[i]) != 1){
			printf("Error reading element %d!\n",i);
			return  FALSE;
		}
	}

	*size = sz;
	return TRUE;
}

int main()
{
	int size;
	int *array;

	array = (int *)malloc(sizeof(int)*150);

	if(readArray(&size, array)==TRUE){
	    printf("%d", computeSum(array,size));
	}

	return 0;
}