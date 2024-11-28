#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// List of C keywords
const char *keywords[] = {
    "auto", "break", "case", "char", "const", "continue", "default", "do",
    "double", "else", "enum", "extern", "float", "for", "goto", "if",
    "int", "long", "register", "return", "short", "signed", "sizeof",
    "static", "struct", "switch", "typedef", "union", "unsigned",
    "void", "volatile", "while"
};

// Function to check if a word is a C keyword
int isKeyword(const char *word) {
    for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++) {
        if (strcmp(keywords[i], word) == 0) {
            return 1;
        }
    }
    return 0;
}

// Helper function to check if a character is a special symbol
int isSpecialSymbol(char ch) {
    return (ch == '(' || ch == ')' || ch == '{' || ch == '}' ||
            ch == '[' || ch == ']' || ch == ',' || ch == ';');
}

// Helper function to check if a character is an operator
int isOperator(char ch) {
    return (ch == '+' || ch == '-' || ch == '*' || ch == '/' ||
            ch == '%' || ch == '=' || ch == '!' || ch == '<' || ch == '>');
}

// Helper function to skip whitespace and comments
void skipWhitespaceAndComments(FILE *fp) {
    char ch = fgetc(fp);

    while (ch == ' ' || ch == '\t' || ch == '\n' || ch == '/') {
        if (ch == '/') { // Check for comments
            ch = fgetc(fp);

            if (ch == '/') { // Single-line comment
                while (ch != '\n' && ch != EOF)
                    ch = fgetc(fp);
            } else if (ch == '*') { // Multi-line comment
                while (1) {
                    ch = fgetc(fp);

                    if (ch == '*' && (ch = fgetc(fp)) == '/') {
                        break;
                    }
                    if (ch == EOF) {
                        return;
                    }
                }
            } else {
                ungetc(ch, fp); // Not a comment, return '/'
                return;
            }
        }
        ch = fgetc(fp);
    }

    if (ch != EOF) {
        ungetc(ch, fp); // Return the non-whitespace character
    }
}

int main() {
    FILE *inputFile = fopen("input.c", "r");
    FILE *outputFile = fopen("output.txt", "w");

    if (!inputFile) {
        printf("Error: Unable to open input file.\n");
        return 1;
    }

    if (!outputFile) {
        printf("Error: Unable to create output file.\n");
        fclose(inputFile);
        return 1;
    }

    char ch, buffer[256];
    int index;

    // Print table header
    fprintf(outputFile, "-------------------------------------\n");
    fprintf(outputFile, "|   TOKEN TYPE   |   TOKEN VALUE    |\n");
    fprintf(outputFile, "-------------------------------------\n");

    // Main loop to process the file
    while ((ch = fgetc(inputFile)) != EOF) {
        skipWhitespaceAndComments(inputFile);

        // Check for identifiers and keywords
        if (isalpha(ch) || ch == '_') {
            index = 0;
            buffer[index++] = ch;

            while (isalnum(ch = fgetc(inputFile)) || ch == '_') {
                buffer[index++] = ch;
            }

            buffer[index] = '\0';
            ungetc(ch, inputFile); // Return the non-alphanumeric character

            if (isKeyword(buffer)) {
                fprintf(outputFile, "|   KEYWORD      |   %-14s |\n", buffer);
            } else {
                fprintf(outputFile, "|   IDENTIFIER   |   %-14s |\n", buffer);
            }
        }

        // Check for numeric literals
        else if (isdigit(ch)) {
            index = 0;
            buffer[index++] = ch;

            while (isdigit(ch = fgetc(inputFile))) {
                buffer[index++] = ch;
            }

            buffer[index] = '\0';
            ungetc(ch, inputFile); // Return the non-digit character
            fprintf(outputFile, "|   LITERAL      |   %-14s |\n", buffer);
        }

        // Check for operators
        else if (isOperator(ch)) {
            char next = fgetc(inputFile);

            if ((ch == '+' && next == '+') || (ch == '-' && next == '-') ||
                (ch == '=' && next == '=') || (ch == '!' && next == '=') ||
                (ch == '&' && next == '&') || (ch == '|' && next == '|')) {
                fprintf(outputFile, "|   OPERATOR     |   %c%c             |\n", ch, next);
            } else {
                ungetc(next, inputFile); // Return the non-operator character
                fprintf(outputFile, "|   OPERATOR     |   %c              |\n", ch);
            }
        }

        // Check for special symbols
        else if (isSpecialSymbol(ch)) {
            fprintf(outputFile, "|   SYMBOL       |   %c              |\n", ch);
        }

        // Handle string literals
        else if (ch == '"') {
            index = 0;
            buffer[index++] = ch;

            while ((ch = fgetc(inputFile)) != '"' && ch != EOF) {
                buffer[index++] = ch;
            }
            buffer[index++] = '"';
            buffer[index] = '\0';

            fprintf(outputFile, "| STRING_LITERAL |   %-14s |\n", buffer);
        }

        // Handle single characters like ';' or ','
        else if (ch == ';' || ch == ',') {
            fprintf(outputFile, "|   SYMBOL       |   %c              |\n", ch);
        }
    }

    // Print table footer
    fprintf(outputFile, "-------------------------------------\n");

    fclose(inputFile);
    fclose(outputFile);

    return 0;
}
