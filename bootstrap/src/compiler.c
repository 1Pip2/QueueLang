#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "compiler.h"

typedef struct Compiler {
    Token* curr;
    TkQueue* tokens;
    char* outputfile;

    char progdef;
    u_int64_t prog_loc;
    char endif;
    u_int64_t* ifrets;
    size_t ifret_num;

    char** vars;
    size_t var_num;

    char** funcs;
    int64_t* func_locs;
    size_t func_num;

    u_int64_t* references_addr;
    u_int64_t* references_func;
    size_t reference_num;
} Compiler;

void eofError(Token* token) {
    PRINT_DEBUG(token);
    printf("Error: Expectd Token. Found EOF\n");
    exit(1);
}

Token* safePop(Compiler* compiler) {
    Token* token = tkQueuePop(compiler->tokens);
    if (token == NULL) {
        eofError(compiler->curr);
    }

    return token;
}

long getFileLen(char* filename) {
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        return 0;
    }

    fseek(fp, 0, SEEK_END);
    long len = ftell(fp);
    fclose(fp);
    return len;
}

void appendByte(char* filename, u_int8_t byte) {
    FILE* fp = fopen(filename, "a");
    if (fp == NULL) {
        printf("Error: Unable to create or edit '%s'\n", filename);
        exit(1);
    }

    fprintf(fp, "%c", byte);
    fclose(fp);
}

void appendQuad(char* filename, u_int64_t data) {
    for (size_t i = 0; i < 8; i++) {
        appendByte(filename, (data >> i * 8) & 0xFF);
    }
}

void setByte(char* filename, u_int8_t byte, size_t pos) {
    FILE *fp = fopen(filename, "r+");
    if (fp == NULL) {
        printf("Error: Unable to create or edit '%s'\n", filename);
        exit(1);
    }

    fseek(fp, pos, SEEK_SET);
    fprintf(fp, "%c", byte);
    fclose(fp);
}

void setQuad(char* filename, u_int64_t data, size_t pos) {
    for (size_t i = 0; i < 8; i++) {
        setByte(filename, (data >> i * 8) & 0xFF, pos + i);
    }
}

char* getOutputFileName(char* filename) {
    size_t filename_len = strlen(filename);
    char* outfile = malloc((filename_len + 4) * sizeof(char));

    strcpy(outfile, filename);
    outfile[filename_len + 0] = '.';
    outfile[filename_len + 1] = 'b';
    outfile[filename_len + 2] = 'c';
    outfile[filename_len + 3] = '\0';

    return outfile;
}

void expectPrimeType(Token* token, TokenType1 type) {
    if (token->type1 != type) {
        PRINT_DEBUG(token);
        printf("SyntaxError\n");
        exit(1);
    }
}
void expectSecType(Token* token, TokenType2 type) {
    if (token->type2 != type) {
        PRINT_DEBUG(token);
        printf("SyntaxError\n");
        exit(1);
    }
}

ByteCode get_op(TokenType2 type) {
    switch (type) {
    case TKTYPE_DO:
        return BC_DO;
    case TKTYPE_DOALL:
        return BC_DOALL;

    case TKTYPE_PLUS:
        return BC_ADD;
    case TKTYPE_MINUS:
        return BC_SUB;
    case TKTYPE_ASTERISK:
        return BC_MULT;
    case TKTYPE_FSLASH:
        return BC_DIV;
    case TKTYPE_PERCENT:
        return BC_MOD;

    case TKTYPE_EQUALS:
        return BC_EQU;
    case TKTYPE_GREATER:
        return BC_GREATER;
    case TKTYPE_LESS:
        return BC_LESS;
    case TKTYPE_GREATEREQU:
        return BC_GREATEREQU;
    case TKTYPE_LESSEQU:
        return BC_LESSEQU;

    case TKTYPE_OR:
        return BC_OR;
    case TKTYPE_XOR:
        return BC_XOR;
    case TKTYPE_AND:
        return BC_AND;
    case TKTYPE_NOT:
        return BC_NOT;
    
    case TKTYPE_DUMP:
        return BC_DUMP;
    case TKTYPE_REQ:
        return BC_REQ;
    case TKTYPE_DUP:
        return BC_DUP;
    case TKTYPE_CPY:
        return BC_CPY;
    case TKTYPE_RM:
        return BC_RM;
    
    case TKTYPE_RET:
        return BC_RET;
    default:
        printf("Unreachable in get_op\n");
        exit(1);
    }
}

void compileBody(Compiler*);
void compileArrayLit(Compiler*);
void compileLit(Compiler* compiler) {
    switch (compiler->curr->type2) {
    case TKTYPE_INT:
        appendByte(compiler->outputfile, BC_DATA);
        appendByte(compiler->outputfile, DT_INT);
        appendQuad(compiler->outputfile, compiler->curr->numdata);
        break;
    case TKTYPE_TRUE:
        appendByte(compiler->outputfile, BC_DATA);
        appendByte(compiler->outputfile, DT_BOOL);
        appendByte(compiler->outputfile, 1);
        break;
    case TKTYPE_FALSE:
        appendByte(compiler->outputfile, BC_DATA);
        appendByte(compiler->outputfile, DT_BOOL);
        appendByte(compiler->outputfile, 0);
        break;
    case TKTYPE_OBRACE:
        compileArrayLit(compiler);
        break;
    default:
        printf("Unreachable in compileLit\n");
    }
}

void compileArrayLit(Compiler* compiler) {
        appendByte(compiler->outputfile, BC_DATA);
        appendByte(compiler->outputfile, DT_ARRAY);

        while ((compiler->curr = safePop(compiler))->type2 != TKTYPE_CBRACE) {
            if (compiler->curr->type1 == _TKTYPE_LIT || compiler->curr->type2 == TKTYPE_OBRACE) {
                compileLit(compiler);
            } else {
                expectPrimeType(compiler->curr, _TKTYPE_LIT);
            }
        }
        appendByte(compiler->outputfile, ENDARRAY);
}


void compileIfStatement(Compiler* compiler) {
    if (compiler->endif) {  // if this is an elif statement
        compiler->endif = 0;  // prevent error in recursive compileBody
        compiler->ifrets = realloc(compiler->ifrets, (++compiler->ifret_num) * sizeof(u_int64_t));
    } else {
        compiler->ifrets = malloc(sizeof(u_int64_t));
        compiler->ifret_num = 1;
    }

    appendByte(compiler->outputfile, BC_JMPNIF);
    size_t ifpos = getFileLen(compiler->outputfile);
    appendQuad(compiler->outputfile, 0);
    
    compileBody(compiler);

    appendByte(compiler->outputfile, BC_JMP);
    size_t ifret = getFileLen(compiler->outputfile);
    appendQuad(compiler->outputfile, 0);
    
    setQuad(compiler->outputfile, ifret + 8, ifpos);  // jump to else
    compiler->endif = 1;
    compiler->ifrets[compiler->ifret_num - 1] = ifret;
}

void compileEndIfStatement(Compiler* compiler) {
    if (compiler->endif == 0) {
        PRINT_DEBUG(compiler->curr);
        printf("SyntaxError: endif without if");
        exit(1);
    }

    for (size_t i = 0; i < compiler->ifret_num; i++) {
        setQuad(compiler->outputfile, getFileLen(compiler->outputfile), compiler->ifrets[i]);
    }    
    compiler->endif = 0;
}

void compileWhileStatement(Compiler* compiler) {
    size_t start_while = getFileLen(compiler->outputfile);
    compileBody(compiler);

    appendByte(compiler->outputfile, BC_JMPNIF);
    size_t end_while = getFileLen(compiler->outputfile);
    appendQuad(compiler->outputfile, 0);

    compileBody(compiler);
    appendByte(compiler->outputfile, BC_JMP);
    appendQuad(compiler->outputfile, start_while);
    
    setQuad(compiler->outputfile, getFileLen(compiler->outputfile), end_while);
}

int64_t findVar(char* var, char** vars, size_t var_num) {
    for (size_t i = 0; i < var_num; i++) {
        if (strcmp(var, vars[i]) == 0) {
            return i;
        }
    }
    return -1;
}
void compileIdentifier(Compiler* compiler) {
    int64_t pos = findVar(compiler->curr->stringdata, compiler->vars, compiler->var_num);
    if (pos != -1) {
        appendByte(compiler->outputfile, BC_VAR);
        appendQuad(compiler->outputfile, (u_int64_t) pos);
    } else {
        PRINT_DEBUG(compiler->curr);
        printf("Error: '%s' is undefined\n", compiler->curr->stringdata);
        exit(1);
    }
}

void compileSetLet(Compiler* compiler) {
    ByteCode bc;
    switch (compiler->curr->type2) {
    case TKTYPE_SET:
        bc = BC_SET;
        break;
    case TKTYPE_LET:
        bc = BC_LET;
        break;
    
    default:
        PRINT_DEBUG(compiler->curr);
        printf("Error: Unreachable in compileSetLet\n");
        exit(1);
    }

    appendByte(compiler->outputfile, bc);
    compiler->curr = safePop(compiler);
    expectSecType(compiler->curr, TKTYPE_OPAREN);

    compiler->curr = safePop(compiler);
    expectPrimeType(compiler->curr, TKTYPE_ID);
    int64_t pos = findVar(compiler->curr->stringdata, compiler->vars, compiler->var_num);
    if (pos == -1) {
        pos = compiler->var_num;
        compiler->vars = realloc(compiler->vars, (++compiler->var_num) * sizeof(char*));
        compiler->vars[pos] = compiler->curr->stringdata;
    }
    appendQuad(compiler->outputfile, pos);


    compiler->curr = safePop(compiler);
    expectSecType(compiler->curr, TKTYPE_CPAREN);
}

int inFuncs(Compiler* compiler, char* str) {
    for (size_t i = 0; i < compiler->func_num; i++) {
        if (strcmp(compiler->funcs[i], str) == 0) {
            compiler->references_func = realloc(compiler->references_func, ++compiler->reference_num * sizeof(u_int64_t));
            compiler->references_func[compiler->reference_num-1] = i;
            return 1;
        }
    }
    return 0;
}

void compileCall(Compiler* compiler) {
    appendByte(compiler->outputfile, BC_CALL);

    compiler->curr = safePop(compiler);
    expectSecType(compiler->curr, TKTYPE_OPAREN);

    compiler->curr = safePop(compiler);
    expectPrimeType(compiler->curr, _TKTYPE_ID);
    if (strcmp(compiler->curr->stringdata, "get") == 0) {
        appendQuad(compiler->outputfile, BUILTIN_GET);
    } else if (strcmp(compiler->curr->stringdata, "append") == 0) {
        appendQuad(compiler->outputfile, BUILTIN_APPEND);
    } else if (strcmp(compiler->curr->stringdata, "pop") == 0) {
        appendQuad(compiler->outputfile, BUILTIN_POP);
    } else if (strcmp(compiler->curr->stringdata, "size") == 0) {
        appendQuad(compiler->outputfile, BUILTIN_SIZE);
    } else if (inFuncs(compiler, compiler->curr->stringdata)) {
        setByte(compiler->outputfile, BC_CALLC, getFileLen(compiler->outputfile) - 1);
        compiler->references_addr = realloc(compiler->references_addr, compiler->reference_num * sizeof(u_int64_t));
        compiler->references_addr[compiler->reference_num-1] = getFileLen(compiler->outputfile);
        appendQuad(compiler->outputfile, 0);
    } else {
        PRINT_DEBUG(compiler->curr);
        printf("NameError: '%s' is undefined\n", compiler->curr->stringdata);
        exit(1);
    }
    compiler->curr = safePop(compiler);
    expectSecType(compiler->curr, TKTYPE_CPAREN);
}

void compileBody(Compiler* compiler) {
    compiler->curr = safePop(compiler);
    expectSecType(compiler->curr, TKTYPE_OBRACK);
    
    while ((compiler->curr = safePop(compiler))->type2 != TKTYPE_CBRACK) {
        if (compiler->curr->type2 == TKTYPE_SET || compiler->curr->type2 == TKTYPE_LET) {
            compileSetLet(compiler);
        } else if (compiler->curr->type2 == TKTYPE_CALL) {
            compileCall(compiler);
        } else if (compiler->curr->type1 == _TKTYPE_OP) {
            appendByte(compiler->outputfile, get_op(compiler->curr->type2));
        } else if (compiler->curr->type1 == _TKTYPE_LIT) {
            compileLit(compiler);
        } else if (compiler->curr->type2 == TKTYPE_OBRACE) {
            compileArrayLit(compiler);
        } else if (compiler->curr->type2 == TKTYPE_IF) {
            compileIfStatement(compiler);
        } else if (compiler->curr->type2 == TKTYPE_ENDIF) {
            compileEndIfStatement(compiler);
        } else if (compiler->curr->type2 == TKTYPE_WHILE) {
            compileWhileStatement(compiler);
        } else if (compiler->curr->type1 == _TKTYPE_ID) {
            compileIdentifier(compiler);
        } else {
            PRINT_DEBUG(compiler->curr);
            printf("SyntaxError\n");
            exit(1);
        }
    }

    if (compiler->endif) {
        PRINT_DEBUG(compiler->curr);
        printf("SyntaxError: Expected endif\n");
        exit(1);
    }
}

void compileProg(Compiler* compiler) {
    if (compiler->progdef) {
        PRINT_DEBUG(compiler->curr);
        printf("Error: Prog already defined\n");
        exit(1);
    }
    compiler->progdef = 1;
    compiler->prog_loc = getFileLen(compiler->outputfile);

    compiler->endif = 0;
    compiler->vars = NULL;
    compiler->var_num = 0;

    compiler->curr = safePop(compiler);
    expectSecType(compiler->curr, TKTYPE_COLON);

    compileBody(compiler);

    appendByte(compiler->outputfile, BC_RET);
}

void getFunDefs(Compiler* compiler) {
    compiler->func_locs = NULL;
    compiler->funcs = NULL;
    compiler->func_num = 0;

    compiler->references_addr = NULL;
    compiler->references_func = NULL;
    compiler->reference_num = 0;

    Token* curr = compiler->tokens->front;
    while (curr != NULL) {
        if (curr->type2 == TKTYPE_FUN) {
            expectPrimeType(curr->last, _TKTYPE_ID);
            compiler->funcs = realloc(compiler->funcs, ++compiler->func_num * sizeof(char*));
            compiler->func_locs = realloc(compiler->func_locs, compiler->func_num * sizeof(int64_t));
            compiler->funcs[compiler->func_num-1] = curr->last->stringdata;
            compiler->func_locs[compiler->func_num-1] = -1;
        }
        curr = curr->last;
    }
}

void compileFun(Compiler* compiler) {
    compiler->endif = 0;
    compiler->vars = NULL;
    compiler->var_num = 0;
    compiler->curr = safePop(compiler);
    
    size_t i;
    for (i = 0; i < compiler->func_num; i++) {
        if (strcmp(compiler->funcs[i], compiler->curr->stringdata) == 0) {
            break;
        }
    }
    
    if (compiler->func_locs[i] != -1) {
        PRINT_DEBUG(compiler->curr);
        printf("NameError: Function already defined\n");
        exit(1);
    }
    compiler->func_locs[i] = getFileLen(compiler->outputfile);

    compiler->curr = safePop(compiler);
    expectSecType(compiler->curr, TKTYPE_COLON);

    compileBody(compiler);

    appendByte(compiler->outputfile, BC_RET);
}

void compileTokens(TkQueue* tokens, char* filename) {
    Compiler* compiler = malloc(sizeof(Compiler));
    char* outfile = getOutputFileName(filename);
    remove(outfile);

    compiler->outputfile = outfile;
    compiler->tokens = tokens;
    compiler->progdef = 0;

    appendByte(compiler->outputfile, BC_JMP);
    appendQuad(compiler->outputfile, 0);
    getFunDefs(compiler);
    while ((compiler->curr = tkQueuePop(compiler->tokens)) != NULL) {
        switch (compiler->curr->type2) {
        case TKTYPE_PROG:
            compileProg(compiler);
            break;
        case TKTYPE_FUN:
            compileFun(compiler);
            break;
        
        default:
            PRINT_DEBUG(compiler->curr);
            printf("SyntaxError\n");
            exit(1);
        }
    }

    if (!compiler->progdef) {
        printf("SyntaxError: Missing 'prog'\n");
        exit(1);
    }

    for (size_t i = 0; i < compiler->reference_num; i++) {
        setQuad(compiler->outputfile, compiler->func_locs[compiler->references_func[i]], compiler->references_addr[i]);
    }
    setQuad(compiler->outputfile, compiler->prog_loc, 1);
}
