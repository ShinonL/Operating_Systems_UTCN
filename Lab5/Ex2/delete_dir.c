#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

#include <string.h>

#include <stdio.h>
#include <stdlib.h>

#define MAX_PATH_LEN 300

int remove_files_in_dir(char *dir_path) {
    DIR* dir;
    struct dirent *dirEntry;
    struct stat inode;
    char name[MAX_PATH_LEN];

    dir = opendir(dir_path);
    if(dir == 0) {
        printf("nonrec: %s\n", dir_path);
        perror("Error opening directory");
        exit(3);
    }

    while((dirEntry = readdir(dir)) != 0) {
        if(strcmp(dirEntry->d_name, "..") == 0 || strcmp(dirEntry->d_name, ".") == 0)
            continue;
        snprintf(name, MAX_PATH_LEN, "%s/%s", dir_path, dirEntry->d_name);
        lstat(name, &inode);
        if(S_ISREG(inode.st_mode)) {
            //printf("%s\n", name);
            unlink(name);
        }
    }
    closedir(dir);
    return 0;
}
int remove_file_tree(char *dir_path) {
    DIR* dir;
    struct dirent *dirEntry;
    struct stat inode;
    char name[MAX_PATH_LEN];

    dir = opendir(dir_path);
    if(dir == 0) {
        printf("%s\n", dir_path);
        perror("Error opening directory");
        exit(4);
    }
    remove_files_in_dir(dir_path);
    while((dirEntry = readdir(dir)) != 0) {
        if(strcmp(dirEntry->d_name, "..") == 0 || strcmp(dirEntry->d_name, ".") == 0)
            continue;
        snprintf(name, MAX_PATH_LEN, "%s/%s", dir_path, dirEntry->d_name);
        lstat(name, &inode);
        if(S_ISDIR(inode.st_mode)) {
            
            //if(unlink(name) == -1)
                remove_file_tree(name);
            rmdir(name);
        }
    }
    closedir(dir);
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

    remove_file_tree(argv[1]);
    return 0;
}