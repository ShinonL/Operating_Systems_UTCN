#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

#include <string.h>

#include <stdio.h>
#include <stdlib.h>

#define MAX_PATH_LEN 300
int size = 0, blockSize = 0;

int total_file_size(char *dir_path) {
    DIR* dir;
    struct dirent *dirEntry;
    struct stat inode;
    char name[MAX_PATH_LEN];

    dir = opendir(dir_path);
    if(dir == 0) {
        perror("Error opening directory");
        exit(3);
    }

    while((dirEntry = readdir(dir)) != 0) {
        if(strcmp(dirEntry->d_name, "..") == 0 || strcmp(dirEntry->d_name, ".") == 0)
            continue;
        snprintf(name, MAX_PATH_LEN, "%s/%s", dir_path, dirEntry->d_name);
        lstat(name, &inode);
        if(S_ISREG(inode.st_mode)) {
            size += inode.st_size;
            blockSize += inode.st_blksize;
        }
    }
    return 0;
}
int total_file_tree_size(char *dir_path) {
    DIR* dir;
    struct dirent *dirEntry;
    struct stat inode;
    char name[MAX_PATH_LEN];

    dir = opendir(dir_path);
    if(dir == 0) {
        perror("Error opening directory");
        exit(4);
    }

    while((dirEntry = readdir(dir)) != 0) {
        if(strcmp(dirEntry->d_name, "..") == 0 || strcmp(dirEntry->d_name, ".") == 0)
            continue;
        snprintf(name, MAX_PATH_LEN, "%s/%s", dir_path, dirEntry->d_name);
        lstat(name, &inode);
        if(S_ISDIR(inode.st_mode)) {
            //if(unlink(name) == -1)
                total_file_tree_size(name);
            total_file_size(name);
        }
    }
    return 0;
}
int main(int argc, char** argv) {
    if(argc != 2) {
        printf("USAGE: ./delete_dir <path>");
        exit(1);
    }

    struct stat fileMetadata;
    if(stat(argv[1], &fileMetadata) < 0) { 
        printf("ERROR (getting info about the file)");
        exit(2);
    }
    if(!S_ISDIR(fileMetadata.st_mode)) {
        printf("%s is not a directory!\n", argv[1]);
        exit(2);
    }

    total_file_tree_size(argv[1]);
    printf("Size: %d\nBlocks: %d\n", size, blockSize);
    return 0;
}