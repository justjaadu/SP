#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_TOKENS 100

int litr_address[100];  
int symb_address[100]; 
bool isStart = true;

void address(const char *filename, int arr[]) {
    FILE *tempFile = fopen(filename, "r"); 
    if (!tempFile) {
        perror("Failed to open file");
        return; 
    }

    char line[1024];
    char *tokens[MAX_TOKENS];
    int tokenCount;

    while (fgets(line, sizeof(line), tempFile) != NULL) {
        line[strcspn(line, "\n")] = 0;  
        char *token = strtok(line, " ");
        tokenCount = 0;

        while (token != NULL && tokenCount < MAX_TOKENS) {
            tokens[tokenCount] = strdup(token);  
            tokenCount++;
            token = strtok(NULL, " ");
        }

        int opcode = atoi(tokens[0]);  
        int address = atoi(tokens[2]);  

        if (opcode < 100) {
            arr[opcode] = address;  
        }

        for (int i = 0; i < tokenCount; i++) {
            free(tokens[i]);
        }
    }

    fclose(tempFile); 
}


void machine(const char *arr[], int size) {  
    FILE *mac = fopen("machine.txt", "a"); 
    if (!mac) {
        perror("Failed to open machine.txt");
        return;
    }
    
    for (int i = 0; i < size; i++) {
        char tempStr[50];
        strcpy(tempStr, arr[i]);
        int number = atoi(tempStr);
        if (isStart && i==1)
        {
            fprintf(mac," 01\t\t%c%c%c",tempStr[3],tempStr[4],tempStr[5]);
            isStart=false;
            continue;
        }
        
        if (i==0 && number!=0)
        {
            fprintf(mac,"%s ",tempStr);
            continue;
        }
        if (i==1)
        {
            fprintf(mac,"%c%c",tempStr[4],tempStr[5]);
        }
      
        int length =strlen(tempStr);
        if(i==2 && length==2){
            fprintf(mac," %c%c",tempStr[0],tempStr[1]);
            continue;

        }else{
            fprintf(mac,"\t");
        }
        char subStr[3];
        strncpy(subStr, &tempStr[3], 2);
         int num2 = atoi(subStr);

        
        if (i==2||i==3)
        {
            if(tempStr[1]=='S'){
                fprintf(mac,"%d",symb_address[num2]);
            }else if (tempStr[1]=='L')
            {
                fprintf(mac,"%d",litr_address[num2]);
            }else{
                fprintf(mac,"%s",tempStr);
            }
            
        }

    }
    fprintf(mac, "\n");
    fclose(mac);
}

int main() {
    FILE *mac = fopen("machine.txt", "w");
    const char *filename = "symbol.txt";  
    const char *filename2 = "Litral.txt";

    address(filename2, litr_address);
    address(filename, symb_address);


    FILE *ifp = fopen("inter_code.txt", "r"); 
    if (!ifp) {
        perror("Failed to open inter_code file");
        return EXIT_FAILURE;
    }

    char line[1024];
    char *tokens[MAX_TOKENS];
    int tokenCount;

    while (fgets(line, sizeof(line), ifp) != NULL) {  
        line[strcspn(line, "\n")] = 0;  
        char *token = strtok(line, " ");
        tokenCount = 0;

        while (token != NULL && tokenCount < MAX_TOKENS) {
            tokens[tokenCount] = strdup(token);  
            tokenCount++;
            token = strtok(NULL, " ");
        }

        machine((const char **)tokens, tokenCount);  

        for (int i = 0; i < tokenCount; i++) {
            free(tokens[i]);  
        }
    }

    fclose(ifp);  

    return EXIT_SUCCESS;
}
