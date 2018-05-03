#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stddef.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <libgen.h>

char *programmName;
char *stroutput;
char *key;
int elementsFromKeyFile;
int pid_num = 0;
int STATUS;

void printError(int _errno, char *name){
    printf("%s %d %s %s\n", programmName, getpid(), strerror(_errno), name);
}

int getFileSize(char *filename) {
    int buffer = -1;
    int descriptor = open(filename, O_RDONLY);
    
    if (descriptor != -1) {
        FILE *file = fdopen(descriptor, "rb");
        if (file) {
            struct stat statistics;
            
            if (fstat(descriptor, &statistics) != -1) {
                buffer = statistics.st_size;
            } else {
                printError(errno, filename);
            }
            if ((fclose(file)) == -1){
                printError(errno, filename);
            }
        }
        close(descriptor);
    } else {
        printError(errno, filename);
    }
    return buffer;
}

char *getFileData(char *filename, char *buffer, int bufferSize){
    FILE *file;
    
    if ((file = fopen(filename, "rb")) != NULL){
        if (fread(buffer, 1, bufferSize, file) != bufferSize){
            printError(errno, filename);
            return NULL;
        }
        if ((fclose(file)) == -1){
            printError(errno, filename);
            return NULL;
        };
    } else {
        printError(errno, filename);
        return NULL;
    }
    return buffer;
}

char *cypherData(char *data, int dataSize, char *key,  int keySize, char *buffer){
    for (int i = 0; i < dataSize; i++) {
        buffer[i] = data[i]^key[i % keySize];
    }
    return buffer;
}

void startCypher(char *directory, char *name){
    stroutput = malloc(strlen(directory) + 50);
    FILE *file;
    strcpy(stroutput, directory);
    strcat(stroutput, "/");
    strcat(stroutput, name);
    int elementsFromInputFile = getFileSize(stroutput);
    char *fileData = malloc(elementsFromInputFile);
    char *buf      = malloc(elementsFromInputFile);
    fileData = getFileData(stroutput, fileData, elementsFromInputFile);
    buf = cypherData(fileData, elementsFromInputFile, key, elementsFromKeyFile, buf);
    printf("%d %s %d \n", getpid(), stroutput, elementsFromInputFile);
    if ((file = fopen(stroutput, "wb")) != NULL){
        if (fwrite(buf, sizeof(char), elementsFromInputFile, file) != elementsFromInputFile) {
            printError(errno, stroutput);
        }
        if ((fclose(file)) == -1) {
            printError(errno, stroutput);
        };
    } else {
        printError(errno, stroutput);
    }
    free(stroutput);
    free(fileData);
    free(buf);
}

int findAnswer(char *currdir, int N) {
    DIR *d;
    struct dirent *dir;
    
    if ((d = opendir(currdir))) {
        while (d) {
            errno = 0;
            if ((dir = readdir(d)) != NULL) {
                if ((dir->d_type == DT_DIR) && (strcmp(dir->d_name, ".") != 0) && (strcmp(dir->d_name, "..") != 0)) {
                    stroutput = malloc(strlen(currdir) + 50);
                    strcpy(stroutput, currdir);
                    strcat(stroutput, "/");
                    strcat(stroutput, dir->d_name);
                    findAnswer(stroutput, N);
                } else {
                    if (dir->d_type == DT_REG) {
                        
                        if (pid_num >= N) {
                            if (waitpid (-1, &STATUS, WNOHANG) == -1){
                                if (errno != ECHILD) {
                                    printError(errno, "");
                                }
                            }
                            pid_num--;
                        }
                        
                        pid_t pid;
                        if ((pid = fork()) == 0){
                            startCypher(currdir, dir->d_name);
                            exit(EXIT_SUCCESS);
                        } else if (pid > 0){
                            pid_num++;
                        } else {
                            printf("%s: Error. %d has not created.", programmName, getpid());
                        }
                    }
                }
            } else {
                if (errno != 0) {
                    printError(errno, currdir);
                }
                break;
            }
        }
        if (closedir(d) == -1) {
            printError(errno, currdir);
        }
    } else {
        printError(errno, currdir);
    }
    return 0;
}

// argv[0] - pragramm name
// argv[1] - working dir
// argv[2] - key file name
// argv[3] - N number of proc. running at the same time

int main(int argc, const char * argv[]) {
    if (argc < 3)
    {
        printf("Wrong number of arguments!\n");
        return 2;
    }
    
    int N = atoi(argv[3]);
    if (N < 1) {
        printf("Programm can not work without processes!\n");
        return 2;
    }
    char *keypath = argv[2];
    char *programmpath = argv[1];
    char temp[100];
    
    strcpy(temp, argv[0]);
    programmName = basename(temp);
    
    elementsFromKeyFile = getFileSize(keypath);
    key = malloc(elementsFromKeyFile);
    key = getFileData(keypath, key, elementsFromKeyFile);
    
    findAnswer(programmpath, N);
    
    while (waitpid (-1, &STATUS, WNOHANG) != -1) { }
    if (errno != ECHILD) {
        printError(errno, "");
    }
    
    free(key);
    return 1;
}
