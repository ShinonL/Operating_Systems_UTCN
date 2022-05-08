#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

#define MAX_PATH_LEN 400
int occurence = 0;

int copy_file(char *src, char *dst) {
    int from, to, nr, nw;
    char buf[MAX_PATH_LEN];
    if ((from = open(src, O_RDONLY)) < 0) {
        printf("Error opening the source file\n");
        exit(2);
    }
    
    if ((to = creat(dst, 0666)) < 0) {
        perror("Error creating the destination file\n");
        exit(3);
    }

    while((nr = read(from, buf, sizeof(buf))) != 0) {
        if (nr < 0) {
            printf("Error reading from the source file\n");
            exit(4);
        }

        if ((nw = write(to, &buf, nr)) < 0) {
            perror("Error writing in destination file\n");
            exit(5);
        }
    }
    
    close(from);
    close(to);
    return 1;
}
int copy_dir(char *src, char *dest) {
    DIR *srcDir;//, *destDir;
    struct dirent *srcEntry;//, *destEntry;
    char name[MAX_PATH_LEN];
    struct stat inode;

    srcDir = opendir(src);
    if(srcDir == 0) {
                printf("%s\n",src);

        printf("ERROR opening directory\n");
        exit(3);
    }

    mkdir(dest, S_IRWXU);
    // destDir = opendir(dest);
    // if(destDir == 0) {
    //     printf("DEST:");
    //     printf("ERROR opening directory\n");
    //     exit(3);
    // }

    while ((srcEntry = readdir(srcDir)) != 0) {
         if(strcmp(srcEntry->d_name, "..") == 0 || strcmp(srcEntry->d_name, ".") == 0)
            continue;
        snprintf(name, MAX_PATH_LEN, "%s/%s",src,srcEntry->d_name);
        lstat (name, &inode);
        if (S_ISDIR(inode.st_mode)) {
            char temp[MAX_PATH_LEN];
            snprintf(temp, MAX_PATH_LEN, "%s/%s.%d", dest, srcEntry->d_name, occurence);
            //mkdir(temp, S_IRWXU);
            copy_dir(name, temp);
        } else if(S_ISREG(inode.st_mode)) {
            char temp[MAX_PATH_LEN];
            snprintf(temp, MAX_PATH_LEN, "%s/%s.%d", dest, srcEntry->d_name, occurence);
            copy_file(name, temp);
        }
    }

    return 0;
}
int search_dir(char *dir_name, char *searched_name, char *dest) {
    DIR* dir;
    struct dirent *dirEntry;
    char name[MAX_PATH_LEN];

    dir = opendir(dir_name);
    if(dir == 0) {
        printf("ERROR opening directory\n");
        exit(3);
    }

    while((dirEntry = readdir(dir)) != 0) {
        if(strcmp(dirEntry->d_name, "..") == 0 || strcmp(dirEntry->d_name, ".") == 0)
            continue;
        snprintf(name, MAX_PATH_LEN, "%s/%s", dir_name, dirEntry->d_name);
        if(strcmp(dirEntry->d_name, searched_name) == 0) {
            printf("Absolute path: %s\n", realpath(name, NULL));
            struct stat fileMetadata;
            if(stat(name, &fileMetadata) < 0) {
                perror("ERROR (getting info about the file)");
                exit(1);
            }
            occurence++;
            printf("Type: ");
            if(S_ISREG(fileMetadata.st_mode)) {
                printf("FILE\n"); 
                char temp[MAX_PATH_LEN] = "";

                snprintf(temp, MAX_PATH_LEN, "%s/%s.%d", dest, dirEntry->d_name, occurence);
                copy_file(name, temp);
            }      
            if(S_ISDIR(fileMetadata.st_mode)) {
                printf("DIRECTORY\n");
                char temp[MAX_PATH_LEN] = "";
                snprintf(temp, MAX_PATH_LEN, "%s/%s.%d", dest, dirEntry->d_name, occurence);
                copy_dir(name, temp);
            }   
            if(S_ISLNK(fileMetadata.st_mode))
                printf("SYMBOLIC LINK\n");
            return 1;
        }
    }
    return 0;
}
int search_tree(char *dir_name, char *searched_name, char *dest) {
    DIR* dir;
    struct dirent *dirEntry;
    char name[MAX_PATH_LEN];

    //printf("dirname: %s\n", dir_name);
    dir = opendir(dir_name);
    if(dir == 0) {
        printf("ERROR opening directory\n");
        exit(3);
    }
    //search_dir(dir_name, searched_name, dest, occurence);
    //printf("dirname: %s\n", dir_name);
    while((dirEntry = readdir(dir)) != 0) {
        if(strcmp(dirEntry->d_name, "..") == 0 || strcmp(dirEntry->d_name, ".") == 0)
            continue;
        snprintf(name, MAX_PATH_LEN, "%s/%s", dir_name, dirEntry->d_name);
        
        struct stat fileMetadata;
        if(stat(name, &fileMetadata) < 0) {
            perror("ERROR (getting info about the file)");
            exit(1);
        }
        //printf("name: %s\n", name);
        if(S_ISDIR(fileMetadata.st_mode)) {
            if(unlink(name) == -1)
                search_tree(name, searched_name, dest);
            search_dir(name, searched_name, dest);
            //if(search_dir(name, searched_name, dest, occurence) == 1)
                //return 1;
        }
    }

    //closedir(dir);
    return 0;
}

int main(int argc, char** argv) {
    if(argc != 4) {
        printf("USAGE: search_files path=<path> name=<file_name> dest=<dest_path>\n");
        exit(1);
    }
    printf("Number of arguments: %d\n", argc);

    char *path = (char *)malloc(MAX_PATH_LEN * sizeof(char));
    char *name = (char *)malloc(MAX_PATH_LEN * sizeof(char));
    char *dest = (char *)malloc(MAX_PATH_LEN * sizeof(char));
    char* token;
    token = strtok(argv[1], "=");
    if(strcmp(token, "path") == 0) {
        token = strtok(NULL, "=");
        strcpy(path, token);
    } else if(strcmp(token, "name") == 0) {
        token = strtok(NULL, "=");
        strcpy(name, token);
    } else if(strcmp(token, "dest") == 0) {
        token = strtok(NULL, "=");
        strcpy(dest, token);
    } else {
        printf("USAGE: search_files path=<path> name=<file_name>\n");
        exit(1);
    }
    token = strtok(argv[2], "=");
    if(strcmp(token, "path") == 0 && path != NULL) {
        token = strtok(NULL, "=");
        strcpy(path, token);
    } else if(strcmp(token, "name") == 0 && name != NULL) {
        token = strtok(NULL, "=");
        strcpy(name, token);
    } else if(strcmp(token, "dest") == 0 && dest != NULL) {
        token = strtok(NULL, "=");
        strcpy(dest, token);
    } else {
        printf("USAGE: search_files path=<path> name=<file_name>\n");
        exit(1);
    }
    token = strtok(argv[3], "=");
    if(strcmp(token, "path") == 0 && path != NULL) {
        token = strtok(NULL, "=");
        strcpy(path, token);
    } else if(strcmp(token, "name") == 0 && name != NULL) {
        token = strtok(NULL, "=");
        strcpy(name, token);
    } else if(strcmp(token, "dest") == 0 && dest != NULL) {
        token = strtok(NULL, "=");
        strcpy(dest, token);
    } else {
        printf("USAGE: search_files path=<path> name=<file_name>\n");
        exit(1);
    }

    struct stat fileMetadata;
    if(stat(path, &fileMetadata) < 0) {
        perror("ERROR (getting info about the file)");
        exit(1);
    }
    if(!S_ISDIR(fileMetadata.st_mode)) {
        perror("ERROR\nThe path is not a directory");
        exit(2);
    }
    if(stat(dest, &fileMetadata) < 0) {
        perror("ERROR (getting info about the file)");
        exit(1);
    }
    if(!S_ISDIR(fileMetadata.st_mode)) {
        perror("ERROR\nThe destination is not a directory");
        exit(2);
    }

    search_tree(path, name, dest);
    if(occurence == 0)
        printf("Name not found\n");
    return 0;
}