#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_TOKENS 100
#define MAX_TOKEN_LENGTH 50 

bool isStart= false;

char *mnt[100];
char *mdt[100];
char *parameter[100];

void address(const char *filename, char *arr[]) {
    FILE *tempFile = fopen(filename, "r");
    if (!tempFile) {
        perror("Failed to open file");
        return;
    }

    char line[1024];
    int lineCount = 0;

    while (fgets(line, sizeof(line), tempFile) != NULL) {
        line[strcspn(line, "\n")] = 0; 

        if (strlen(line) > 3) {
            arr[lineCount] = strdup(line + 3);  
        } else {
            arr[lineCount] = strdup("");  
        }
        lineCount++;
    }

    fclose(tempFile);
}
void expanssion(const char *arr[], int size,FILE *exp){
    for (int i = 0; i < size; i++) {
        if (strcmp(arr[i], "START") == 0) {
            isStart=true;
        }
        if (isStart)
        {
            char fs[50];  
            strcpy(fs, arr[i]);

            if (strcmp(fs, "XYZ") == 0) {
            for (int i = 2; i < 5; i++)
            {
                fprintf(exp,"%s\n",mdt[i]);
            }
              break;
        }
        fprintf(exp,"%s ",fs);
        }

    } 
    if(isStart){
        fprintf(exp,"\n");
    }
    

}

int main() {
    const char *filename = "mdt.txt";
    const char *filename2 = "mnt.txt";
    const char *filename3 = "parameter.txt";
    FILE *exp = fopen("expand.txt","w");

    // Populate arrays with file data
    address(filename, mdt);
    address(filename2, mnt);
    address(filename3, parameter);

    FILE *input = fopen("input.txt", "r");
    if (!input) {
        perror("Failed to open input file");
        return EXIT_FAILURE;
    }

    char line[1024];
    char *tokens[MAX_TOKENS];
    int tokenCount;
    while (fgets(line, sizeof(line), input) != NULL) {
        line[strcspn(line, "\n")] = 0;  
        char *token = strtok(line, " ");
        tokenCount = 0;

        while (token != NULL && tokenCount < MAX_TOKENS) {
            tokens[tokenCount] = strdup(token); 
            tokenCount++;
            token = strtok(NULL, " ");
        }

        expanssion(tokens, tokenCount,exp);

        for (int i = 0; i < tokenCount; i++) {
            free(tokens[i]);
        }
    }


    fclose(input);
    return EXIT_SUCCESS;
}
