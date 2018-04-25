#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stddef.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>

const char *programmName;
char *stroutput;
char *key;
int elementsFromKeyFile;


void printError(int _errno, char *name){
    printf("%s: %s %s\n", programmName, strerror(_errno), name);
}

void dump_buffer(char *buffer, int bufferSize)
{
    for (int c=0;c<bufferSize;c++)
    {
        printf("%.2X ", (int)buffer[c]);
        
        // put an extra space between every 4 bytes
        if (c % 4 == 3)
        {
            printf(" ");
        }
        
        // Display 16 bytes per line
        if (c % 16 == 15)
        {
            //printf("\n");
        }
    }
    printf("\n");
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
            };
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
    int j = 0;

    
    for (int i = 0; i < dataSize; i++) {
        buffer[i] = data[i]^key[j++ % keySize];
    }
    
//    printf("key:    ");
//    dump_buffer(key, keySize);
//    printf("data:   ");
//    dump_buffer(data, dataSize);
//    printf("result: ");
//    dump_buffer(buffer, dataSize);
    
    return buffer;
}

int findAnswer(char *currdir, int N) {
    DIR *d;
    struct dirent *dir;
    
    if ((d = opendir(currdir)))
    {
        while (d)
        {
            errno = 0;
            if ((dir = readdir(d)) != NULL)
            {
                if (
                    (dir->d_type == DT_DIR) && (strcmp(dir->d_name, ".") != 0) && (strcmp(dir->d_name, "..") != 0)
                    )
                {
                    stroutput = malloc(strlen(currdir) + 50);
                    
                    strcpy(stroutput, currdir);
                    strcat(stroutput, "/");
                    strcat(stroutput, dir->d_name);
                    // printf("DIRECTORY: %s\n", stroutput);
                    
                    findAnswer(stroutput, N);
                } else {
                    if (dir->d_type == DT_REG) {
                        stroutput = malloc(strlen(currdir) + 50);
                        FILE *file;
                    
                        strcpy(stroutput, currdir);
                        strcat(stroutput, "/");
                        strcat(stroutput, dir->d_name);
                        // printf("FILE: %s\n", stroutput);
                        
                        int elementsFromInputFile = getFileSize(stroutput);
                        char *fileData = malloc(elementsFromInputFile);
                        char *buf      = malloc(elementsFromInputFile);
                        
                        fileData = getFileData(stroutput, fileData, elementsFromInputFile);
                        
                        buf = cypherData(fileData, elementsFromInputFile, key, elementsFromKeyFile, buf);
                        
                        if ((file = fopen(stroutput, "wb")) != NULL){
                            if (fwrite(buf, sizeof(char), elementsFromInputFile, file) != elementsFromInputFile){
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
                }
            } else {
                if (errno != 0) {
                    printf("l2_v2.c: Cannot read directory");
                    printf("\t%s\n", currdir);
                }
                break;
            }
        }
        if (closedir(d) == -1) {
            printf("l2_v2.c: Cannot close directory");
            printf("\t%s\n", currdir);
        }
    } else {
        printf("l2_v2.c: Cannot open directory");
        printf("\t%s\n", currdir);
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
        printf("l2_v2.c: Wrong number of arguments!\n");
        return 2;
    }
    
    programmName = argv[0];

    elementsFromKeyFile = getFileSize(argv[2]);
    key = malloc(elementsFromKeyFile);
    key = getFileData(argv[2], key, elementsFromKeyFile);
    
    findAnswer(argv[1], argv[3]);
    
    free(key);
    return 1;
}