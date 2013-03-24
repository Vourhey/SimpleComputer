/* Compiler for Simple language */
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "stack.h"

struct TableEntry {
    char *symbol;
    TokenType type;      /* LINE_NUMBER, CONSTANT or VARIABLE */
    int location;   /*  0 to 99 */
};

int findInTable(struct TableEntry *table, struct Token token);
int addToTable(struct TableEntry *table, struct Token token, int l);
int priority(const char *o1, const char *o2);
char *generateTempName();

int main(int argc, char **argv)
{
    FILE *in = fopen(argv[1], "r");
    struct Tokens *tokens = parse(in);

    struct TableEntry table[100];
    struct Token flags[100];    /* for second pass */

    {   /* clear */
        printf("Clear... ");

        int l = 0;
        while(l < 100) {
            table[l].symbol = 0;
            table[l].type = OTHER;
            table[l].location = 0;
            ++l;
        }
    }

    {
        struct Token tmp;
        tmp.lexem = NULL;
        tmp.type = OTHER;

        int i = 0;
        while(i < 100) {
            flags[i] = tmp;
            ++i;
        }

        printf("finish\n");
    }

    int memory[100] = { 0 };
    int instruction = 0;
    int variable    = 99;

    struct Token token;
    int i = 0;

    printf("start compilation:\n");
    printf("tokens->size: %d\n", tokens->size);
    while(i < tokens->size) {
        printf("debuging...\n");
        token = tokens->tokens[i];


        printf("get token\n");

        switch (token.type) {
        case LINE_NUMBER:
            addToTable(table, token, instruction);
            break;
        case INPUT:
            memory[instruction] = 1000;
            /* next token */
            token = tokens->tokens[++i];
            if(token.type == VARIABLE) {
                int f = findInTable(table, token);
                if(f == -1) {
                    f = addToTable(table, token, variable--);
                }
                memory[instruction] += table[f].location;
            } else {
               ; /* compilation error: syntax error */
            }
            
            instruction++;
            break;
        case PRINT:
            memory[instruction] = 1100;
            token = tokens->tokens[++i];
            if(token.type == VARIABLE) {
                int f = findInTable(table, token);
                if(f == -1) {
                    f = addToTable(table, token, variable--);
                }
                memory[instruction] += table[f].location;
            } else {
                ;   /* compilation error */
            }
            instruction++;
            break;
        case IFGOTO: {
            printf("IFGOTO:\n");
            
            struct Token var1;
            struct Token var2;
            struct Token gototoken;
            struct Token goconst;
            int f;
            var1 = tokens->tokens[++i];
            if(var1.type == VARIABLE || var1.type == CONSTANT) {
                f = findInTable(table, var1);
                if(f == -1) {
                    f = addToTable(table, var1, variable--);
                }
                if(var1.type == CONSTANT) {
                    memory[table[f].location] = atoi(var1.lexem);
                }
            } else {
                ; /* compilation error */
            }

            token = tokens->tokens[++i];  /* ожидается оператор сравнения */
            if(token.type != COMPARE) {
                ; /* compilation error */
            }

            var2 = tokens->tokens[++i];
            if(var2.type == VARIABLE || var2.type == CONSTANT) {
                f = findInTable(table, var2);
                if(f == -1) {
                    f = addToTable(table, var2, variable--);
                }
                if(var2.type == CONSTANT) {
                    memory[table[f].location] = atoi(var2.lexem);
                }
            } else {
                ; /* compilation error */
            }

            gototoken = tokens->tokens[++i];
            if(gototoken.type != GOTO) {
                ; /* compilation error */
            }

            goconst = tokens->tokens[++i];
            if(goconst.type == CONSTANT) { 
                /* In future: goto variable  */
                goconst.type = LINE_NUMBER;
            } else {
                ; /* compilation error */
            }

            /* instruction generation */
            if(!strcmp(token.lexem, "<=")) {
                f = findInTable(table, var1);
                memory[instruction++] = 2000 + table[f].location;
                f = findInTable(table, var2);
                memory[instruction++] = 3100 + table[f].location;
                f = findInTable(table, goconst);
                memory[instruction] = 4200;
                memory[instruction + 1] = 4100;
                if(f == -1) {
                    flags[instruction]     = goconst;
                    flags[instruction + 1] = goconst;
                } else {
                    memory[instruction] += table[f].location;
                    memory[instruction + 1] += table[f].location;
                }
                instruction += 2;
            } else if(!strcmp(token.lexem, ">=")) {
                f = findInTable(table, var2);
                memory[instruction++] = 2000 + table[f].location;
                f = findInTable(table, var1);
                memory[instruction++] = 3100 + table[f].location;
                f = findInTable(table, goconst);
                memory[instruction] = 4200;
                memory[instruction + 1] = 4100;
                if(f == -1) {
                    flags[instruction] = goconst;
                    flags[instruction + 1] = goconst;
                } else {
                    memory[instruction]     += table[f].location;
                    memory[instruction + 1] += table[f].location;
                }
                instruction += 2;
            } else if(!strcmp(token.lexem, "!=")) {
                f = findInTable(table, var1);
                memory[instruction++] = 2000 + table[f].location;
                f = findInTable(table, var2);
                memory[instruction++] = 3100 + table[f].location;
                f = findInTable(table, goconst);
                memory[instruction++] = 4200 + instruction + 1;
                memory[instruction] = 4000;
                if(f == -1) {
                    flags[instruction] = goconst;
                } else {
                    memory[instruction] += table[f].location;
                }
                ++instruction;
            } else if(!strcmp(token.lexem, "==")) {
                printf("               ==                !!!! \n");
                f = findInTable(table, var1);
                memory[instruction++] = 2000 + table[f].location;
                f = findInTable(table, var2);
                memory[instruction++] = 3100 + table[f].location;
                f = findInTable(table, goconst);
                memory[instruction] = 4200;
                if(f == -1) {
                    flags[instruction] = goconst;
                } else {
                    memory[instruction] += table[f].location;
                }
                ++instruction;
            } else if(!strcmp(token.lexem, ">")) {
                f = findInTable(table, var2);
                memory[instruction++] = 2000 + table[f].location;
                f = findInTable(table, var1);
                memory[instruction++] = 3100 + table[f].location;
                f = findInTable(table, goconst);
                memory[instruction] = 4100;
                if(f == -1) {
                    flags[instruction] = goconst;
                } else {
                    memory[instruction] += table[f].location;
                }
                ++instruction;
            } else if(!strcmp(token.lexem, "<")) {
                f = findInTable(table, var1);
                memory[instruction++] = 2000 + table[f].location;
                f = findInTable(table, var2);
                memory[instruction++] = 3100 + table[f].location;
                f = findInTable(table, goconst);
                memory[instruction] = 4100;
                if(f == -1) {
                    flags[instruction] = goconst;
                } else {
                    memory[instruction] += table[f].location;
                }
                ++instruction;
            } /* else {}   never happen */

            break;
        }
        case LET: {
            struct Stack *tokensStack;
            tokensStack = init(tokensStack);
            struct Stack *resultStack;
            resultStack = init(resultStack);

            /* todo: check two variable below */
            struct Token leftOperand = tokens->tokens[++i];
            struct Token equal = tokens->tokens[++i];

            int f;

            /* ... */
            /* required: var = some expression */

            token = tokens->tokens[++i];
            while(token.type != LINE_NUMBER) {
                if(token.type == VARIABLE || token.type == CONSTANT) {
                    pushIntoStack(tokensStack, token);
                } else if(token.type == BRACE) {
                    if(*token.lexem == '(') {
                        pushIntoStack(resultStack, token);
                    } else {
                        token = popFromStack(resultStack);
                        while(*token.lexem != '(') {
                            pushIntoStack(tokensStack, token);
                            token = popFromStack(resultStack);
                        }
                    }
                } else if(token.type == ARITHMETIC) {
                    if(resultStack->size == 0) {    /* empty */
                        printf("pushIntoStack: arithmetic\n");
                        pushIntoStack(resultStack, token);
                    } else {
                        struct Token t = topStack(resultStack);
                        while(priority(token.lexem, t.lexem) <= 0) {
                            t = popFromStack(resultStack);
                            pushIntoStack(tokensStack, t);
                            t = topStack(resultStack);
                            if(t.lexem == NULL) {
                                break;
                            }
                        }
                        pushIntoStack(resultStack, token);
                    }
                } else {
                    ;   /* compilatoin error */
                }

                token = tokens->tokens[++i];
            }

            while(resultStack->size) {
                token = popFromStack(resultStack);
                pushIntoStack(tokensStack, token);
            }

            clearStack(resultStack);
            resultStack = init(resultStack);

{
            printf("Tokens stack: ");
            int i=tokensStack->size - 1;
            while(i >= 0) {
                printf("%s ", tokensStack->top[i].lexem);
                --i;
            }
}

            int i = 0;
            while(i < tokensStack->size) {
                token = tokensStack->top[i];/*= popFromStack(tokensStack); */
                
                if(token.type == VARIABLE || token.type == CONSTANT) {
                    pushIntoStack(resultStack, token);
                } else if(token.type == ARITHMETIC) {
                    struct Token varB = popFromStack(resultStack);
                    printf("varB.lexem: %s\n", varB.lexem);
                    struct Token varA = popFromStack(resultStack);
                    printf("varA.lexem: %s\n", varA.lexem);

                    if(!(varA.lexem && varB.lexem)) {
                        ;   /* error */
                    }

                    struct Token tmpvar;   /* generate temporary variable */
                    tmpvar.lexem = generateTempName();
                    tmpvar.type  = VARIABLE;
                    addToTable(table, tmpvar, variable--);

                    f = findInTable(table, varA);
                    if(f == -1) {
                        f = addToTable(table, varA, variable);
                        if(varA.type == CONSTANT) {
                            printf("here constant\n");
                            memory[variable] = atoi(varA.lexem);
                        }
                        --variable;
                    }
                    memory[instruction++] = 2000 + table[f].location;
                    f = findInTable(table, varB);
                    if(f == -1) {
                        f = addToTable(table, varB, variable);
                        if(varB.type == CONSTANT) {
                            printf("here constant\n");
                            memory[variable] = atoi(varB.lexem);
                        }
                        --variable;
                    }
                    if(!strcmp(token.lexem, "+")) {    
                        memory[instruction++] = 3000 + table[f].location; 
                    } else if(!strcmp(token.lexem, "-")) {
                        memory[instruction++] = 3100 + table[f].location;
                    } else if(!strcmp(token.lexem, "*")) {
                        memory[instruction++] = 3300 + table[f].location;
                    } else if(!strcmp(token.lexem, "/")) {
                        memory[instruction++] = 3200 + table[f].location;
                    }
                    /* ... else ... */

                    printf("find tmpvar\n");
                    f = findInTable(table, tmpvar);
                    printf("success\n");
                    memory[instruction++] = 2100 + table[f].location;

                    pushIntoStack(resultStack, tmpvar);
                }
                ++i;
            }

            token = popFromStack(resultStack);
            f = findInTable(table, token);
            memory[instruction++] = 2000 + table[f].location;
            f = findInTable(table, leftOperand);
            if(f == -1) {
                f = addToTable(table, leftOperand, variable--);
            }
            memory[instruction++] = 2100 + table[f].location;

            break;
        }
        case GOTO:
            memory[instruction] = 4000;
            token = tokens->tokens[++i];
            if(token.type == CONSTANT) {
                token.type = LINE_NUMBER;
                int f = findInTable(table, token);
                if(f == -1) {
                    flags[instruction] = token;
                } else {
                    memory[instruction] += f;
                }
            } else {
                ; /* compilation error */
            }
            instruction++;
            break;
        case END:
            memory[instruction] = 4300;
            ++instruction;
            break;
/*        case COMPARE:
        case EQUALS:
        case OTHER:
        case ARITHMETIC: */
        default:
            break;  /* compilation error */
        }
        ++i;
    }

    /* second pass */
    i = 0;
    while(i < 100) {
        if(flags[i].lexem) {
            int f = findInTable(table, flags[i]);
            memory[i] += table[f].location;
        }
        ++i;
    }

    printf("End compilation!\n");

    freeTokens(tokens);

    printf("Write to file... \n");
    
    FILE *out = fopen("out.sml", "wb");
    fwrite(memory, sizeof(int), 100, out);

    fclose(in);
    fclose(out);

    printf("end\n");

    return 0;
}

int addToTable(struct TableEntry *table, struct Token token, int l)
{
    static int c = 0;

    printf("addToTable: %d\n", c);
    table[c].symbol = token.lexem;
    table[c].type = token.type;
    table[c].location = l;

    return c++;
}

int findInTable(struct TableEntry *table, struct Token token)
{
    printf("findInTable: ");
    int n = 0;
    while(n < 100) {
        if(table[n].type == token.type) {
            if(!strcmp(table[n].symbol, token.lexem)) {
                printf("%d\n", n);
                return n;
            }
        }
        ++n;
    }

    printf("-1\n");
    return -1; /* addToTable(table, token, 0); */
}

/*
 * if o1 > o2 then 1
 * if o1 < o2 then -1
 * else 0
 */
int priority(const char *o1, const char *o2)
{
    /* +, -, *, / */

    if(*o1 == '+' || *o1 == '-') {
        if(*o2 == '*' || *o2 == '/') {
            return -1;
        }
        return 0;
    }

    if(*o2 == '+' || *o2 == '-') {
        return 1;
    }

    return 0;
}

/* for temporary variable */
char *generateTempName()
{
    char *varname = (char*)malloc(sizeof(char) * 4);

    srand(time(NULL));

    int i = 0;
    while(i < 3) {
        varname[i] = (char) (rand() % 10 + '0');
        printf("%c", varname[i]);
        ++i;
    }
    varname[3] = '\0';
    printf("\n");

    return varname;
}

