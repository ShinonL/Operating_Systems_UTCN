// C program libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// directory libraries
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

#define MAX_PATH_LEN 300

typedef struct {
    char name[11];
    int type;
    int size;
    int offset;
} SectionT;

void listDir(char *path, char *startsWith, int size) {
    DIR* dir;
    struct dirent *dirEntry;
    struct stat inode;
    char name[MAX_PATH_LEN];

    dir = opendir(path);
    if(dir == 0) {
        printf("ERROR opening directory\n");
        exit(4);
    }

    while((dirEntry = readdir(dir)) != 0) {
        if(strcmp(dirEntry->d_name, "..") == 0 || strcmp(dirEntry->d_name, ".") == 0)
            continue;

        snprintf(name, MAX_PATH_LEN, "%s/%s", path, dirEntry->d_name);
        lstat(name, &inode);
        if(size >= 0) {
            if(S_ISREG(inode.st_mode)) {
                if(size >= 0 && inode.st_size <= size) 
                    strcpy(name, "");
            } else strcpy(name, "");
        } else {
            if(strcmp(startsWith, "") != 0 && strncmp(dirEntry->d_name, startsWith, strlen(startsWith)) != 0)
                strcpy(name, "");
        }
        
        if(strcmp(name, "") != 0)
            printf("%s\n", name);
    }
    closedir(dir);
}
void listDirRec(char *path, char *startsWith, int size) {
    DIR* dir;
    struct dirent *dirEntry;
    struct stat inode;
    char name[MAX_PATH_LEN];

    dir = opendir(path);
    if(dir == 0) {
        printf("ERROR opening directory\n");
        exit(4);
    }

    while((dirEntry = readdir(dir)) != 0) {
        if(strcmp(dirEntry->d_name, "..") == 0 || strcmp(dirEntry->d_name, ".") == 0)
            continue;
        snprintf(name, MAX_PATH_LEN, "%s/%s", path, dirEntry->d_name);
        lstat(name, &inode);
        if(S_ISDIR(inode.st_mode)) {
            listDirRec(name, startsWith, size);
        } 
        if(size >= 0) {
            if(S_ISREG(inode.st_mode)) {
                if(size >= 0 && inode.st_size <= size) 
                    strcpy(name, "");
            } else strcpy(name, "");
        } else {
            if(strcmp(startsWith, "") != 0 && strncmp(dirEntry->d_name, startsWith, strlen(startsWith)) != 0)
                strcpy(name, "");
        }
        if(strcmp(name, "") != 0)
            printf("%s\n", name);
    }
    closedir(dir);
}
void setParameters(char *str, char *path, bool *recursive, char *startsWith, int *size) {
    char* token = strtok(str, "=");
    if(strcmp(token, "path") == 0) {
        token = strtok(NULL, "=");
        strcpy(path, token);
    } else if(strcmp(token, "recursive") == 0) {
        *recursive = true;
    } else if(strcmp(token, "size_greater") == 0) {
        token = strtok(NULL, "=");
        sscanf(token, "%d", size);
    } else if(strcmp(token, "name_starts_with") == 0) {
        token = strtok(NULL, "=");
        strcpy(startsWith, token);
    }
}
SectionT* getSections(char* path, int *numberOfSections, int *version) {
    struct stat fileMetadata;
    if(stat(path, &fileMetadata) < 0) {
        printf("ERROR\ngetting info about the file\n");
        exit(4);
    }
    if(!S_ISREG(fileMetadata.st_mode)) {
        printf("ERROR\n%s is not a path to a file!\n", path);
        exit(4);
    }

    int file = open(path, O_RDONLY);
    if(file < 0) {
        printf("ERROR\ncould not open file\n");
        exit(4);
    }

    // get magic
    char* magic = (char *)malloc(4 * sizeof(char));
    if(read(file, magic, 4) < 0) {
        printf("ERROR\ncould not read file\n");
        exit(4);
    }
    if(strcmp(magic, "nlPC") != 0) {
        printf("ERROR\nwrong magic\n");
        free(magic);
        exit(4);
    }
    free(magic);

    // skip header_size
    lseek(file, 2, SEEK_CUR);

    // get version
    *version = 0;
    unsigned char* verString = (unsigned char *)malloc(5 * sizeof(unsigned char));
    if(read(file, verString, 4) < 0) {
        printf("ERROR\ncould not read file\n");
        free(verString);
        exit(4);
    }
    for(int i = 0; i < 4; i++) {
        *version += (int)verString[i]*(1<<(i*8));
    }
    if(*version < 69 || *version > 131) {
        printf("ERROR\nwrong version\n");
        free(verString);
        exit(4);
    }

    // get number of sections
    char character;
    if(read(file, &character, 1) < 0) {
        printf("ERROR\ncould not read file\n");
        free(verString);
        exit(4);
    }
    *numberOfSections = (int)character;
    if(*numberOfSections < 2 || *numberOfSections > 16) {
        printf("ERROR\nwrong sect_nr\n");
        free(verString);
        exit(4);
    }

    // sections
    SectionT *sections = (SectionT *)malloc(*numberOfSections * sizeof(SectionT));
    for(int index = 0; index < *numberOfSections; index++) {
        // get section name
        char* name = (char *)malloc(10 * sizeof(char));
        if(read(file, name, 10) < 0) {
            printf("ERROR\ncould not read file\n");
            free(verString);
            free(name);
            exit(4);
        }
        strcpy(sections[index].name, name);
        free(name);

        // get section type
        if(read(file, &character, 1) < 0) {
            printf("ERROR\ncould not read file\n");
            free(verString);
            exit(4);
        }
        int sectType = (int)character;
        if(sectType != 15 && sectType != 89) {
            printf("ERROR\nwrong sect_types\n");
            free(verString);
            free(sections);
            exit(4);
        }
        sections[index].type = sectType;
        
        // get section offset
        int offset = 0;
        if(read(file, verString, 4) < 0) {
            printf("ERROR\ncould not read file\n");
            free(verString);
            exit(4);
        }
        //printf("offset: %s\n", verString);
        for(int i = 0; i < 4; i++) {
            offset += (int)verString[i]*(1<<(i*8));
        }
        sections[index].offset = offset;
        // get section size
        int sectSize = 0;
        if(read(file, verString, 4) < 0) {
            printf("ERROR\ncould not read file\n");
            free(verString);
            exit(4);
        }
        //printf("secSize: %s\n", verString);
        for(int i = 0; i < 4; i++) {
            sectSize += (int)verString[i]*(1<<(i*8));
        }
        sections[index].size = sectSize;
    }
    free(verString);
    close(file);
    return sections;
}
SectionT* getFindAllSections(char* path, int *numberOfSections, int *version) {//, int *headerSize) {
    struct stat fileMetadata;
    if(stat(path, &fileMetadata) < 0) {
        printf("ERROR\ngetting info about the file\n");
        return NULL;
    }
    if(!S_ISREG(fileMetadata.st_mode)) {
        printf("ERROR\n%s is not a path to a file!\n", path);
        return NULL;
    }

    int file = open(path, O_RDONLY);
    if(file < 0) {
        printf("ERROR\ncould not open file\n");
        return NULL;
    }

    // get magic
    char* magic = (char *)malloc(4 * sizeof(char));
    if(read(file, magic, 4) < 0) {
        printf("ERROR\ncould not read file\n");
        free(magic);
        return NULL;
    }
    free(magic);

    // skip header_size
    lseek(file, 2, SEEK_CUR);
    
    // get version
    *version = 0;
    unsigned char* verString = (unsigned char *)malloc(4 * sizeof(unsigned char));
    if(read(file, verString, 4) < 0) {
        printf("ERROR\ncould not read file\n");
        free(verString);
        return NULL;
    }
    for(int i = 0; i < 4; i++) {
        *version += (int)verString[i]*(1<<(i*8));
    }
    if(*version < 69 || *version > 131) {
        free(verString);
        return NULL;
    }

    // get number of sections
    char character;
    if(read(file, &character, 1) < 0) {
        printf("ERROR\ncould not read file\n");
        free(verString);
        return NULL;
    }
    *numberOfSections = (int)character;
    if(*numberOfSections < 2 || *numberOfSections > 16) {
        free(verString);
        return NULL;
    }

    // sections
    SectionT *sections = (SectionT *)malloc(*numberOfSections * sizeof(SectionT));
    for(int index = 0; index < *numberOfSections; index++) {
        // skip section name
        char* name = (char *)malloc(10 * sizeof(char));
        if(read(file, name, 10) < 0) {
            printf("ERROR\ncould not read file\n");
            free(verString);
            free(name);
            return NULL;
        }
        strcpy(sections[index].name, name);
        free(name);

        // get section type
        if(read(file, &character, 1) < 0) {
            printf("ERROR\ncould not read file\n");
            free(verString);
            return NULL;
        }
        int sectType = (int)character;
        if(sectType != 15 && sectType != 89) {
            free(verString);
            return NULL;
        }
        sections[index].type = sectType;

        // get section offset
        int offset = 0;
        if(read(file, verString, 4) < 0) {
            printf("ERROR\ncould not read file\n");
            free(verString);
            return NULL;
        }
        for(int i = 0; i < 4; i++) {
            offset += (int)verString[i]*(1<<(i*8));
        }
        sections[index].offset = offset;
        
        // get section size
        int sectSize = 0;
        if(read(file, verString, 4) < 0) {
            printf("ERROR\ncould not read file\n");
            free(verString);
            return NULL;
        }
        for(int i = 0; i < 4; i++) {
            sectSize += (int)verString[i]*(1<<(i*8));
        }
        sections[index].size = sectSize;
    }
    free(verString);
    close(file);
    return sections;
}
int getNumberOfLines(char *path, SectionT section) {
    int lineCount = 1, charCount = 0;
    int file = open(path, O_RDONLY);
    if(file < 0) {
        printf("ERROR\ncould not open file\n");
        exit(6);
    }
    lseek(file, section.offset, SEEK_SET);
    char letter;
    while(read(file, &letter, 1) > 0 && charCount <= section.size) {
        if(letter == '\n') {
            lineCount++;
        }
        charCount++;
    }
    close(file);
    return lineCount;
}
void findall(char *path) {
    DIR* dir;
    struct dirent *dirEntry;
    struct stat inode;
    char name[MAX_PATH_LEN];

    dir = opendir(path);
    if(dir == 0) {
        printf("ERROR opening directory\n");
        exit(4);
    }
    while((dirEntry = readdir(dir)) != 0) {
        if(strcmp(dirEntry->d_name, "..") == 0 || strcmp(dirEntry->d_name, ".") == 0)
            continue;
        snprintf(name, MAX_PATH_LEN, "%s/%s", path, dirEntry->d_name);
        lstat(name, &inode);
        bool ok = true;
        if(S_ISDIR(inode.st_mode)) {
            findall(name);
            ok = false;
        } else {
            //printf("%s\n", name);
            int numberOfSections, version, validSections = 0;
            SectionT *sections = getFindAllSections(name, &numberOfSections, &version);
            if(sections != NULL) {
                
                for(int i = 0; i < numberOfSections; i++) {
                    if(getNumberOfLines(name, sections[i]) == 13)
                        validSections++;
                    //printf("%d %d\n", getNumberOfLines(name, sections[i]), validSections);
                }
                //printf("\n");
                
                if(validSections < 4) {
                    ok = false;
                }
                free(sections);
            } else ok = false;
        }
        if(ok)
            printf("%s\n", name);
    }
    closedir(dir);
}
int main(int argc, char **argv){
    if(argc < 2) {
        printf("USAGE: ./a1 [OPTIONS] [PARAMETERS]\n");
        exit(1);
    }

    // variant
    if(strcmp(argv[1], "variant") == 0) {
        if(argc != 2) {
            printf("Error: ./a1 variant does not accept other parameters\n");
            exit(2);
        }
        printf("25380\n");
        return 0;
    }

    // list
    if(strcmp(argv[1], "list") == 0) {
        if(argc == 2) {
            printf("USAGE: ./a1 list path=<path> [recursive] [filters]\n");
            exit(3);
        }

        bool recursive = false;
        char path[MAX_PATH_LEN] = "", startsWith[MAX_PATH_LEN] = "";
        int size = -1;
        
        setParameters(argv[2], path, &recursive, startsWith, &size);
        
        if(argv[3] != NULL) {
            setParameters(argv[3], path, &recursive, startsWith, &size);
        }
        if(argv[4] != NULL) {
            setParameters(argv[4], path, &recursive, startsWith, &size);
        }
        if(argv[5] != NULL) {
            setParameters(argv[5], path, &recursive, startsWith, &size);
        }

        struct stat fileMetadata;
        if(stat(path, &fileMetadata) < 0) {
            printf("ERROR\ngetting info about the file\n");
            exit(3);
        }
        if(!S_ISDIR(fileMetadata.st_mode)) {
            printf("ERROR\n%s is not a directory!\n", path);
            exit(3);
        }

        printf("SUCCESS\n");
        if(recursive)
            listDirRec(path, startsWith, size);
        else listDir(path, startsWith, size);
        return 0;
    }

    // parse
    if(strcmp(argv[1], "parse") == 0) {
        if(argc != 3) {
            printf("USAGE: ./a1 list path=<path>\n");
            exit(4);
        }

        char path[MAX_PATH_LEN];
        char* token = strtok(argv[2], "=");
        if(strcmp(token, "path") == 0) {
            token = strtok(NULL, "=");
            strcpy(path, token);
        } else {
            printf("USAGE: ./a1 list path=<path>\n");
            exit(4);
        }

        int numberOfSections, version;//, headerSize;
        SectionT* sections = getSections(path, &numberOfSections, &version);//, &headerSize);

        printf("SUCCESS\n");
        printf("version=%d\nnr_sections=%d\n", version, numberOfSections);
        for(int i = 0; i < numberOfSections; i++) {
            printf("section%d: %s %d %d\n", i+1, sections[i].name, sections[i].type, sections[i].size);
        }

        free(sections);
        return 0;
    }

    // extract
    if(strcmp(argv[1], "extract") == 0) {
        if(argc != 5) {
            printf("USAGE: ./a1 extract path=<file_path> section=<nr> line=<nr>\n");
            exit(5);
        }

        char path[MAX_PATH_LEN];
        char* token = strtok(argv[2], "=");
        if(strcmp(token, "path") == 0) {
            token = strtok(NULL, "=");
            strcpy(path, token);
        }

        int sectionNumber;
        token = strtok(argv[3], "=");
        if(strcmp(token, "section") == 0) {
            token = strtok(NULL, "=");
            sscanf(token, "%d", &sectionNumber);
        }
        
        int lineNumber;
        token = strtok(argv[4], "=");
        if(strcmp(token, "line") == 0) {
            token = strtok(NULL, "=");
            sscanf(token, "%d", &lineNumber);
        }
        
        int numberOfSections, version;//, headerSize;
        SectionT* sections = getSections(path, &numberOfSections, &version);//, &headerSize);
        if(sectionNumber > numberOfSections || sectionNumber <= 0) {
            printf("ERROR\ninvalid section\n");
            exit(5);
        }

        int file = open(path, O_RDONLY);
        if(file < 0) {
            printf("ERROR\ncould not open file\n");
            exit(5);
        }
        lseek(file, sections[sectionNumber - 1].offset, SEEK_SET);
        unsigned char letter;
        char *line = (char *)malloc(sections[sectionNumber - 1].size * sizeof(char));
        int lineCount = 1, length = 0, charCount = 0;
        bool success = false;
        while(read(file, &letter, 1) > 0 && charCount <= sections[sectionNumber - 1].size) {
            if(letter == '\n') {
                if(lineCount == lineNumber) {
                    success = true;
                    break;
                } else {
                    strcpy(line, "");
                    length = 0;
                    lineCount++;
                }
            } else {
                line[length] = letter;
                length++;
            }
            charCount++;
        }
        if(success) {
            printf("SUCCESS\n");
            while(length > 0) {
                length--;
                printf("%c", line[length]);
            }
            printf("\n");
        } else {
            printf("ERROR\ninvalid line\n");
            free(sections);
            free(line);
            exit(5);
        }
        free(sections);
        free(line);
        return 0;
    }

    // findall
    if(strcmp(argv[1], "findall") == 0) {
        char path[MAX_PATH_LEN];
        char* token = strtok(argv[2], "=");
        if(strcmp(token, "path") == 0) {
            token = strtok(NULL, "=");
            strcpy(path, token);
        }

        struct stat fileMetadata;
        if(stat(path, &fileMetadata) < 0) {
            printf("ERROR\ngetting info about the file\n");
            exit(6);
        }
        if(!S_ISDIR(fileMetadata.st_mode)) {
            printf("ERROR\ninvalid directory path\n");
            exit(6);
        }

        printf("SUCCESS\n");
        findall(path);
        return 0;
    }

    printf("ERROR\nInvalid command");
    return -1;
}