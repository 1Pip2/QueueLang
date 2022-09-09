#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"

#define MAX_LINE_LEN 200
typedef struct Lines {
    char** lines;
    size_t size;
} Lines;

Lines* linesInit(void) {
    Lines* new = malloc(sizeof(Lines));
    new->size = 0;
    new->lines = NULL;

    return new;
}

char* linesAppendNew(Lines* lines) {
    char* new = malloc(MAX_LINE_LEN * sizeof(char));
    
    lines->lines = realloc(lines->lines, (++lines->size) * sizeof(char*));
    lines->lines[lines->size - 1] = new;

    return new;
}

Lines* getFilePointerLines(FILE* fp) {
    Lines* lines = linesInit();

    char* curr = linesAppendNew(lines);
    for (size_t i = 0; fgets(curr, MAX_LINE_LEN, fp) != NULL; i++) {
        if (strlen(curr) > 198) {
            printf("Error in line %lu: Line length may not exeed 197\n", i);
            exit(1);
        }

        lines->lines[i] = realloc(curr, (strlen(curr) + 1) * sizeof(char));  // resize array
        curr = linesAppendNew(lines);
    }

    return lines;
}

Lines* getLines(char* filename) {
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("ERROR: Unable to open '%s'\n", filename);
        exit(1);
    }

    Lines* lines = getFilePointerLines(fp);
    
    fclose(fp);
    return lines;
}

typedef struct Lexer {
    char* filename;
    Lines* lines;
    size_t line_num;
    size_t index;

    TkQueue* tokens;
} Lexer;

TkQueue* tkQueueInit(void) {
    TkQueue* new = malloc(sizeof(TkQueue));
    new->front = NULL;
    new->back = NULL;

    return new;
}

void tkQueueAppend(Lexer* lexer, TokenType1 type1, TokenType2 type2, char* str, u_int64_t num) {
    Token* new = malloc(sizeof(Token));
    new->type1 = type1;
    new->type2 = type2;

    new->stringdata = str;
    new->numdata = num;

    new->filename = lexer->filename;
    new->line = lexer->line_num;

    new->last = NULL;

    if (lexer->tokens->back == NULL) {
        lexer->tokens->front = new;
        lexer->tokens->back = new;
    } else {
        lexer->tokens->back->last = new;
        lexer->tokens->back = new;
    }
}

Token* tkQueuePop(TkQueue* queue) {
    if (queue->front == NULL) {
        return NULL;
    }

    Token* tmp = queue->front;
    queue->front = queue->front->last;
    if (tmp == queue->back) {
        queue->back = NULL;
    }

    return tmp;
}

char isLetter(char ch) {
    if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch == '_')) {
        return 1;
    }
    return 0;
}

char isNumeric(char ch) {
    if (ch >= '0' && ch <= '9') {
        return 1;
    }
    return 0;
}

void appendIdentifier(Lexer* lexer, char* str) {
    if (strcmp(str, "fun") == 0) {
        tkQueueAppend(lexer, _TKTYPE_KEYWORD, TKTYPE_FUN, NULL, 0);
        free(str);
    } else if (strcmp(str, "prog") == 0) {
        tkQueueAppend(lexer, _TKTYPE_KEYWORD, TKTYPE_PROG, NULL, 0);
        free(str);
    } else if (strcmp(str, "if") == 0) {
        tkQueueAppend(lexer, _TKTYPE_KEYWORD, TKTYPE_IF, NULL, 0);
        free(str);
    } else if (strcmp(str, "endif") == 0) {
        tkQueueAppend(lexer, _TKTYPE_KEYWORD, TKTYPE_ENDIF, NULL, 0);
        free(str);
    } else if (strcmp(str, "while") == 0) {
        tkQueueAppend(lexer, _TKTYPE_KEYWORD, TKTYPE_WHILE, NULL, 0);
        free(str);

    } else if (strcmp(str, "do") == 0) {
        tkQueueAppend(lexer, _TKTYPE_OP, TKTYPE_DO, NULL, 0);
        free(str);
    } else if (strcmp(str, "req") == 0) {
        tkQueueAppend(lexer, _TKTYPE_OP, TKTYPE_REQ, NULL, 0);
        free(str);
    } else if (strcmp(str, "dup") == 0) {
        tkQueueAppend(lexer, _TKTYPE_OP, TKTYPE_DUP, NULL, 0);
        free(str);
    } else if (strcmp(str, "cpy") == 0) {
        tkQueueAppend(lexer, _TKTYPE_OP, TKTYPE_CPY, NULL, 0);
        free(str);
    } else if (strcmp(str, "rm") == 0) {
        tkQueueAppend(lexer, _TKTYPE_OP, TKTYPE_RM, NULL, 0);
        free(str);

    } else if (strcmp(str, "or") == 0) {
        tkQueueAppend(lexer, _TKTYPE_OP, TKTYPE_OR, NULL, 0);
        free(str);
    } else if (strcmp(str, "xor") == 0) {
        tkQueueAppend(lexer, _TKTYPE_OP, TKTYPE_XOR, NULL, 0);
        free(str);
    } else if (strcmp(str, "and") == 0) {
        tkQueueAppend(lexer, _TKTYPE_OP, TKTYPE_AND, NULL, 0);
        free(str);
    } else if (strcmp(str, "not") == 0) {
        tkQueueAppend(lexer, _TKTYPE_OP, TKTYPE_NOT, NULL, 0);
        free(str);

    } else if (strcmp(str, "let") == 0) {
        tkQueueAppend(lexer, _TKTYPE_OP, TKTYPE_LET, NULL, 0);
        free(str);
    } else if (strcmp(str, "set") == 0) {
        tkQueueAppend(lexer, _TKTYPE_OP, TKTYPE_SET, NULL, 0);
        free(str);
    } else if (strcmp(str, "call") == 0) {
        tkQueueAppend(lexer, _TKTYPE_OP, TKTYPE_CALL, NULL, 0);
        free(str);
    } else if (strcmp(str, "ret") == 0) {
        tkQueueAppend(lexer, _TKTYPE_OP, TKTYPE_RET, NULL, 0);
        free(str);

    } else if (strcmp(str, "True") == 0) {
        tkQueueAppend(lexer, _TKTYPE_LIT, TKTYPE_TRUE, NULL, 0);
        free(str);
    } else if (strcmp(str, "False") == 0) {
        tkQueueAppend(lexer, _TKTYPE_LIT, TKTYPE_FALSE, NULL, 0);
        free(str);
    } else {
        tkQueueAppend(lexer, _TKTYPE_ID, TKTYPE_ID, str, 0);
    }
}

Lexer* lexerInit(char* filename, Lines* lines) {
    Lexer* new = malloc(sizeof(Lexer));
    
    new->filename = filename;
    new->lines = lines;
    new->line_num = 0;

    new->tokens = tkQueueInit();
    return new;
}

void lexIdentifier(Lexer* lexer) {
    char* line = lexer->lines->lines[lexer->line_num];
    char* str = malloc(sizeof(char));
    *str = line[lexer->index];

    size_t i;
    for (i = 1; isLetter(line[lexer->index + i]) || isNumeric(line[lexer->index + i]); i++) {
        str = realloc(str, (i + 1) * sizeof(char));
        str[i] = line[lexer->index + i];
    }
    str[i] = '\0';
    appendIdentifier(lexer, str); 

    lexer->index += i - 1;
}

void lexIntLit(Lexer* lexer) {
    char* line = lexer->lines->lines[lexer->line_num];
    char* str = malloc(sizeof(char));
    *str = line[lexer->index];

    size_t i;
    for (i = 1; isNumeric(line[lexer->index + i]); i++) {
        str = realloc(str, (i + 1) * sizeof(char));
        str[i] = line[lexer->index + i];
    }
    str[i] = '\0';
    tkQueueAppend(lexer, _TKTYPE_LIT, TKTYPE_INT, NULL, atol(str));

    lexer->index += i;
}

void lexChar(Lexer* lexer) {
    char* line = lexer->lines->lines[lexer->line_num];


    if (isNumeric(line[lexer->index]) || (line[lexer->index] == '-' && isNumeric(line[lexer->index + 1]))) {
        lexIntLit(lexer);
    }
    
    switch (line[lexer->index]) {
        case ' ':
            return;
        case '\n':
            return;
        case '\t':
            return;

        case ';':
            tkQueueAppend(lexer, _TKTYPE_OP, TKTYPE_DOALL, NULL, 0);
            return;

        case '+':
            tkQueueAppend(lexer, _TKTYPE_OP, TKTYPE_PLUS, NULL, 0);
            return;
        case '-':
            tkQueueAppend(lexer, _TKTYPE_OP, TKTYPE_MINUS, NULL, 0);
            return;
        case '*':
            tkQueueAppend(lexer, _TKTYPE_OP, TKTYPE_ASTERISK, NULL, 0);
            return;
        case '/':
            tkQueueAppend(lexer, _TKTYPE_OP, TKTYPE_FSLASH, NULL, 0);
            return;
        case '%':
            tkQueueAppend(lexer, _TKTYPE_OP, TKTYPE_PERCENT, NULL, 0);
            return;

        case '=':
            tkQueueAppend(lexer, _TKTYPE_OP, TKTYPE_EQUALS, NULL, 0);
            return;
        case '>':
            if (line[lexer->index + 1] == '=') {
                tkQueueAppend(lexer, _TKTYPE_OP, TKTYPE_GREATEREQU, NULL, 0);
                lexer->index++;
                return;
            }

            tkQueueAppend(lexer, _TKTYPE_OP, TKTYPE_GREATER, NULL, 0);
            return;
        case '<':
            if (line[lexer->index + 1] == '=') {
                tkQueueAppend(lexer, _TKTYPE_OP, TKTYPE_LESSEQU, NULL, 0);
                lexer->index++;
                return;
            }

            tkQueueAppend(lexer, _TKTYPE_OP, TKTYPE_LESS, NULL, 0);
            return;
        

        case '.':
            tkQueueAppend(lexer, _TKTYPE_OP, TKTYPE_DUMP, NULL, 0);
            return;
        
        case ':':
            tkQueueAppend(lexer, _TKTYPE_SEPARATOR, TKTYPE_COLON, NULL, 0);
            return;
        case '[':
            tkQueueAppend(lexer, _TKTYPE_SEPARATOR, TKTYPE_OBRACK, NULL, 0);
            return;
        case ']':
            tkQueueAppend(lexer, _TKTYPE_SEPARATOR, TKTYPE_CBRACK, NULL, 0);
            return;
        case '{':
            tkQueueAppend(lexer, _TKTYPE_SEPARATOR, TKTYPE_OBRACE, NULL, 0);
            return;
        case '}':
            tkQueueAppend(lexer, _TKTYPE_SEPARATOR, TKTYPE_CBRACE, NULL, 0);
            return;
        case '(':
            tkQueueAppend(lexer, _TKTYPE_SEPARATOR, TKTYPE_OPAREN, NULL, 0);
            return;
        case ')':
            tkQueueAppend(lexer, _TKTYPE_SEPARATOR, TKTYPE_CPAREN, NULL, 0);
            return;
    }

    if (isLetter(line[lexer->index])) {
        lexIdentifier(lexer);
    } else {
            printf("In %s::%lu\n", lexer->filename, lexer->line_num);
        printf("ERROR: Unknown Symbol '%c'\n", line[lexer->index]);
        exit(1);
    }
}

void lexLine(Lexer* lexer) {
    for (lexer->index = 0; lexer->lines->lines[lexer->line_num][lexer->index] != '\0'; lexer->index++) {
        lexChar(lexer);
    }
}

TkQueue* lexLines(Lines* lines, char* filename) {
    Lexer* lexer = lexerInit(filename, lines);
    for (; lexer->line_num < lines->size; lexer->line_num++) {
        lexLine(lexer);
    }

    return lexer->tokens;
}

TkQueue* lexFile(char* filename) {
    return lexLines(getLines(filename), filename);
}
