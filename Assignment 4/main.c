#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_TOKENS 100  
#define MAX_TOKEN_LENGTH 50 

bool isMacro = false;
bool isFirst = false;
bool isFl = false;
int mnti = 1;     
int mdti = 1;     
int pni = 1;
char *pnt[100];
int lc = 0;

void mcro(const char *arr[], int size, FILE *mnt, FILE *mdt, FILE *pntab) {
    lc += 1;

    for (int i = 0; i < size; i++) {
        if (strcmp(arr[i], "MACRO") == 0) {
            printf("MACRO\n");
            fprintf(mdt, "%02d MACRO\n", mdti++);
            isMacro = true;
            isFl = true;
            isFirst = true;
            continue;
        }

        if (isFl) {
            if (isFirst) {
                fprintf(mnt, "%02d %s %02d\n", mnti++, arr[i], mdti-1);
                fprintf(mdt, "%02d %s ", mdti++, arr[i]);
                isFirst = false;
            } else {
                fprintf(pntab, "%02d %s\n", pni, arr[i]);
                fprintf(mdt, "%s ", arr[i]);
                pnt[pni++] = strdup(arr[i]);
                

                if (i == size - 1) {
                    fprintf(mdt, "\n%02d ", mdti++);
                    isFl = false;
                    return;
                }
            }
        }

        if (isMacro && !isFl) {
            fprintf(mdt, "%s ", arr[i]);
            if (i == size - 1) {
                fprintf(mdt, "\n%02d ", mdti++);
            }
        }

        if (strcmp(arr[i], "MEND") == 0) {
            printf("MEND\n");

            isMacro = false;
            isFl = false;
        }
    }
}

int main() {
    FILE *input = fopen("input.txt", "r");
    FILE *mdt = fopen("mdt.txt", "w");
    FILE *mnt = fopen("mnt.txt", "w");
    FILE *pntab = fopen("parameter.txt", "w");

    if (!input || !mdt || !mnt || !pntab) {
        perror("Error opening file");
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

        mcro((const char **)tokens, tokenCount, mnt, mdt, pntab);

        for (int i = 0; i < tokenCount; i++) {
            free(tokens[i]);
        }
    }

    fclose(input);
    fclose(mdt);
    fclose(mnt);
    fclose(pntab);

    return EXIT_SUCCESS;
}
