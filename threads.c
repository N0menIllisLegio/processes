#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
int getFileSize() {
    
    void *buffer = NULL;
    
    int descriptor = open("filename", O_RDONLY);
#include <sys/stat.h>
#include <stddef.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

const char *programmName;

void printError(int _errno){
    printf("%s: %s\n", programmName, strerror(_errno));
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
                printError(errno);
            }
            if ((fclose(file)) == -1){
                printError(errno);
            };
        }
        close(descriptor);
    } else {
        printError(errno);
    }
    
    return buffer;
}

char *getFileData(char *filename, char *buffer, int bufferSize){
    FILE *file;
    
    if ((file = fopen(filename, "rb")) != NULL){
        if (fread(buffer, 1, bufferSize, file) != bufferSize){
            printError(errno);
            return NULL;
        }
        if ((fclose(file)) == -1){
            printError(errno);
            return NULL;
        };
    } else {
        printError(errno);
        return NULL;
    }
    return buffer;
}

char XOR(char a, char b){
    if (a == b){
        return '0';
    } else{
        return '1';
    };
}

char *cypherData(char *data, int dataSize, char *key,  int keySize){
    int j = 0;
    char *r = alloca(dataSize);
    
    for (int i = 0; i < dataSize; i++) {
        r[i] = data[i]^key[j++ % keySize];
    }
    printf("key:    ");
    dump_buffer(key, keySize);
    printf("data:   ");
    dump_buffer(data, dataSize);
    printf("result: ");
    dump_buffer(r, dataSize);
    
    strcpy(data, r);
    return data;
}

int main(int argc, const char * argv[]) {
    programmName = argv[0];
    FILE *file;
    int elementsFromInputFile;
    
    
    int elementsFromKeyFile = getFileSize("key.txt");
    char *key = (char *)malloc(elementsFromKeyFile);
    key = getFileData("key.txt", key, elementsFromKeyFile);
    
    
    elementsFromInputFile = getFileSize("input.txt");
    char *fileData = (char *)malloc(elementsFromInputFile);
    fileData = getFileData("input.txt", fileData, elementsFromInputFile);
    
    fileData = cypherData(fileData, elementsFromInputFile, key, elementsFromKeyFile);
    
    file = fopen("output.txt", "wb");
    fwrite(fileData, sizeof(char), elementsFromInputFile, file);
    fclose(file);
    free(fileData);
    
    
    
    free(key);
    return 1;
}