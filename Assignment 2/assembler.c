#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_TOKENS 100  
#define MAX_TOKEN_LENGTH 50 
bool isPot = false;

int lc = 0; 
int o = 0; 
int a =0;  
int oplit = 0;  
int opsym = 0;  
char *litr[100];  
char *symb[100];  
int litr_address[100];  
int symb_address[100];  
bool isDl=false;


void findRegister(const char *word, FILE *ifp) {
    const char *reg[] = {"AREG,", "BREG,", "CREG,", "DREG,"};
    int reg_size = sizeof(reg) / sizeof(reg[0]);

    for (int i = 0; i < reg_size; i++) {
        if (strcmp(word, reg[i]) == 0) {
            fprintf(ifp, "%02d ", i + 1); 
        }
    }
}


void findLiteral(const char *word, FILE *ifp, FILE *literal) {
    if (strchr(word, '=') != NULL) { 

        for (int i = a; i < oplit; i++) {
            if (strcmp(litr[i], word) == 0) {
                fprintf(ifp, " (L,%02d)", i + 1);
                return;
            }
        }

        litr[oplit] = strdup(word);
        litr_address[oplit] = -1; 
        oplit += 1;
        fprintf(ifp, " (L,%02d)", oplit);
        fprintf(literal, "%02d %s %d\n", oplit, word, -1);
    }
}

void findSymbol(const char *word, FILE *ifp, FILE *symbol) {
    if (isDl)
    {
        return;
    }
    
    const char *pot[] = {"START", "END", "EQU", "ORIGIN", "LTORG", "DS", "DC"};
    int pot_size = sizeof(pot) / sizeof(pot[0]);
    const char *mot[] = {"MOVER", "MOVEM", "ADD", "SUB", "MUL", "DIV", "BC", "COMP", "PRINT", "READ"};
    int mot_size = sizeof(mot) / sizeof(mot[0]);
    const char *reg[] = {"AREG,", "BREG,", "CREG,", "DREG,"};
    int reg_size = sizeof(reg) / sizeof(reg[0]);
    int number1 = atoi(word);
    if (number1 == 0) { 
        bool founds = false;
        bool foundl = false;
        bool foundm = false;
        bool foundp = false;
        bool foundr = false;
        for (int i = 0; i < opsym; i++) {
            if (strcmp(word, symb[i]) == 0) {
                founds = true;  
                fprintf(ifp, " (S,%02d)", i + 1);
                break;
            }
        }
        for (int i = 0; i < oplit; i++) {
            if (strcmp(word, litr[i]) == 0) {
                foundl = true; 
            
                break;
            }
        }
        for (int i = 0; i < mot_size; i++) {
            if (strcmp(word, mot[i]) == 0) {
                foundm = true; 
                break;
            }
        }
        for (int i = 0; i < pot_size; i++) {
            if (strcmp(word, pot[i]) == 0) {
                foundp = true;
                break;
            }
        }
        for (int i = 0; i < reg_size; i++) {
            if (strcmp(word, reg[i]) == 0) {
                foundr = true;
                break;
            }
        }

        if (!founds&&!foundl&&!foundm&&!foundp&&!foundr) {
            symb[opsym] = strdup(word);
            symb_address[opsym] = lc;
            opsym += 1;
            fprintf(symbol, "%02d %s %d\n", opsym, word, lc);
            if (word[strlen(word) - 1] == ':') {
                printf("."); 

            }else{
            fprintf(ifp, " (S,%02d)", opsym);  
            }
        }
    }
}

void findMot(const char *arr[], int size, FILE *ifp, FILE *literal, FILE *symbol) {
    const char *mot[] = {"MOVER", "MOVEM", "ADD", "SUB", "MUL", "DIV", "BC", "COMP", "PRINT", "READ"};
    int mot_size = sizeof(mot) / sizeof(mot[0]);

    for (int i = 0; i < size; i++) {
        int is_mot = 0;
        for (int j = 0; j < mot_size; j++) {
            if (strcmp(arr[i], mot[j]) == 0) {
                fprintf(ifp, "%03d) (IS,%02d) ", lc, j + 1);  
                lc += 1; 
                is_mot = 1;
                break;
            }
        }

        if (!is_mot) {
            findRegister(arr[i], ifp);  
            findLiteral(arr[i], ifp, literal);  
            findSymbol(arr[i], ifp, symbol);  
        }

        if (i == size - 1) {
            fprintf(ifp, "\n"); 
        }
    }
}

void findPot(const char *arr[], int size, FILE *ifp, FILE *literal,FILE *pool) {
    const char *pot[] = {"START", "END", "EQU", "ORIGIN", "LTORG", "DS", "DC"};
    int pot_size = sizeof(pot) / sizeof(pot[0]);

    for (int i = 0; i < size; i++) {
        if (strcmp(arr[i], "START") == 0) {
            fprintf(ifp, "\t(AD,01) (C,%s)\n", arr[i + 1]);  
            lc = atoi(arr[i + 1]);  
            isPot=true;
        } else if (strcmp(arr[i], "END") == 0) {
            if (o<oplit)
            {
                for (int j = o; j < oplit; j++) {
                if (litr_address[j] == -1) {
                    litr_address[j] = lc;
                    char fs[50];  
                    strcpy(fs, litr[j]);
                    int num = atoi(&fs[2]); 
                    fprintf(ifp,"%03d) (AD,02)\t %03d",lc,num);
                    fprintf(literal, "%02d %s %d\n", j + 1, litr[j], lc); 
                    lc += 1; 
                }
            isPot=true;
            }
            }else{
                 fprintf(ifp,"%03d) (AD,02)\t",lc);
            }
            
        } else if (strcmp(arr[i], "EQU") == 0) {
            isPot=true;
            return;
        } else if (strcmp(arr[i], "ORIGIN") == 0) {
            lc = atoi(arr[i + 1]);
            a = oplit-1; 
            isPot=true;
            break;
        } else if (strcmp(arr[i], "LTORG") == 0) {

            fprintf(pool,"%d\n",o);
            for (int j = o; j < oplit; j++) {
                if (litr_address[j] == -1) {
                    litr_address[j] = lc;
                    fprintf(literal, "%02d %s %d\n", j + 1, litr[j], lc);  
                    char fs[50];  
                    strcpy(fs, litr[j]);
                    int num = atoi(&fs[2]);  
                    fprintf(ifp, "%03d) (AD,05)\t %03d\n",lc, num);  
                    lc += 1;  
                }               
            }
            isPot=true;
            o = oplit; 
        } else if (strcmp(arr[i], "DS") == 0) {
            isDl=true;
            char temp[50];  
            strcpy(temp, arr[i+1]);
            int num = atoi(temp); 
            fprintf(ifp,"%03d) (DL,01)\t %03d\n",lc,num);
            char fs[50]; 
            strcpy(fs, arr[i - 1]); 
             for (int i = 0; i < opsym; i++) {
            if (strcmp(fs, symb[i]) == 0) {
                symb_address[i]=lc;
                break;
            }
            isPot=true;
        }

            lc+=1;
        } else if (strcmp(arr[i], "DC") == 0) {
            isDl=true;
            char temp[50];  
            strcpy(temp, arr[i+1]);
            int num = atoi(temp); 
            fprintf(ifp,"%03d) (DL,02)\t %03d\n",lc,num);
            char fs[50];  
            strcpy(fs, arr[i - 1]); 
             for (int i = 0; i < opsym; i++) {
            if (strcmp(fs, symb[i]) == 0) {
                symb_address[i]=lc;
                break;
            }
        }
            lc+=1;
            isPot=true;
        }
    }
}

int main() {
    FILE *input = fopen("input.txt", "r");
    if (input == NULL) {
        perror("Error opening input file");
        return EXIT_FAILURE;
    }

    FILE *ifp = fopen("inter_code.txt", "w");
    FILE *symbol = fopen("symbol.txt", "w");
    FILE *literal = fopen("Litral.txt", "w");
    FILE *pool = fopen("pool.txt", "w");
   

    if (!ifp || !symbol || !literal) {
        perror("Error opening output files");
        return EXIT_FAILURE;
    }

    char line[1024];
    char *tokens[MAX_TOKENS];
    int tokenCount;

    while (fgets(line, sizeof(line), input) != NULL) {
        isPot=false;
        isDl=false;
        line[strcspn(line, "\n")] = 0;  
        char *token = strtok(line, " ");
        tokenCount = 0;

        while (token != NULL && tokenCount < MAX_TOKENS) {
            tokens[tokenCount] = strdup(token); 
            tokenCount++;
            token = strtok(NULL, " ");
        }

        findPot(tokens, tokenCount, ifp, literal,pool);
        if(!isPot){
            findMot(tokens, tokenCount, ifp, literal, symbol);
        }
        
        literal = fopen("Litral.txt", "w");
        for (int i = 0; i < oplit; i++) {
        fprintf(literal,"%02d %s %d\n", i+1 ,litr[i],litr_address[i]); 
        } 
        symbol = fopen("symbol.txt", "w");
        for (int i = 0; i < opsym; i++) {
        fprintf(symbol,"%02d %s %d\n", i+1 ,symb[i],symb_address[i]); 
        } 

        for (int i = 0; i < tokenCount; i++) {
            free(tokens[i]);
        }
    }

    fclose(input);
    fclose(ifp);
    fclose(symbol);
    fclose(literal);

    return 0;
}
