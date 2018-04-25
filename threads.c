#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
int getFileSize() {
    
    void *buffer = NULL;
    
    int descriptor = open("filename", O_RDONLY);
    
    if (descriptor != -1) {
        FILE *file = fdopen(descriptor, "rb");
        
        if (file) {
            struct stat statistics;
            
            if (fstat(descriptor, &statistics) != -1) {
                buffer = (char*)malloc(statistics.st_size);
            }
            fclose(file);
        }
        close(descriptor);
    }
    
    if (!buffer) {
        free(buffer);
    }
    
    return 0;
}

int startCypher(){
    return 1;
}

char XOR(char a, char b){
    if (a == b){
        return '0';
    } else{
        return '1';
    };
}

int main(int argc, const char * argv[]) {
//    pid_t pid;       /* идентификатор процесса */
//    printf ("Пока всего один процесс\n");
//    pid = fork ();     /* Создание нового процесса */
    
    FILE *file;
    char key[100], fileData[100], newFileData[100];
    unsigned long elementsReadFromKeyFile, elementsReadFromInputFile;
    
    file = fopen("key.txt", "rb");
    elementsReadFromKeyFile   = fread(key, sizeof(char), 100, file);
    fclose(file);
    
    file = fopen("outputKey.txt", "wb");
    fwrite(newFileData, sizeof(char), elementsReadFromKeyFile, file);
    fclose(file);
    
    file = fopen("input.txt", "rb");
    elementsReadFromInputFile = fread(fileData, sizeof(char), 100, file);
    fclose(file);
    
    file = fopen("outputInput.txt", "wb");
    fwrite(newFileData, sizeof(char), elementsReadFromKeyFile, file);
    fclose(file);
    
    int i = 0;
    for (i = 0; i < elementsReadFromInputFile; i++) {
        newFileData[i] = XOR(fileData[i], key[i % elementsReadFromKeyFile]);
    }
    
    file = fopen("output.txt", "wb");
    fwrite(newFileData, sizeof(char), --i, file);
    fclose(file);
    
    return 1;
}
