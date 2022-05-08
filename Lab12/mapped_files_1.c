#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	int fd;
	off_t size, i;
	char *data = NULL;
    if(argc != 2) {
		fprintf(stderr, "Usage: %s <file_name>\n", argv[0]);
		return 1;
	}

	/// modified
	fd = open(argv[1], O_RDWR);

	if(fd == -1) {
		perror("Could not open input file");
		return 1;
	}
	size = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);

	/// modified
	data = (char*)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if(data == MAP_FAILED) {
		perror("Could not map file");
		close(fd);
		return 1;
	}

    printf("Before: ");
    for(i=size-1; i>=0; i--) {
		printf("%c", data[i]);
	}

	/// modified
	for(i=size-1; i>=(size-1)/2; i--) {
		char temp = data[i];
		data[i]=data[size-i-1];
		data[size-i-1]=temp;
	}

    printf("\nAfter: ");
    for(i=size-1; i>=0; i--) {
		printf("%c", data[i]);
	}
	printf("\n");

	munmap(data, size);
	close(fd);

	return 0;
}
