#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

struct Token getNextToken(char * line);
struct Tokens *addToken(struct Tokens *, struct Token);
int isKeyword(char *);

struct Tokens *parse(FILE *in)
{
    printf("Start parsing...\n");

    struct Tokens *tokens = (struct Tokens*) malloc(sizeof(struct Tokens));
    tokens->tokens = (struct Token*)malloc(sizeof(struct Token)*10);
    tokens->size = 0;
    tokens->allocated = 10;
    printf("malloc for tokens\n");

    struct Token token;
    char *currentLine = (char*)malloc(sizeof(char) * 80);

    currentLine = fgets(currentLine, 80, in);

    while(currentLine != NULL) {
        printf("Current line is \"%s\"\n", currentLine);
        token = getNextToken(currentLine);
        while(token.lexem != NULL) {
            printf("Token is \"%s\"\n", token.lexem);
            tokens = addToken(tokens, token);
            printf("Token added...\n");
            token = getNextToken(NULL);
        }

        currentLine = fgets(currentLine, 80, in);
    }

    free(currentLine);

    printf("... end parsing\n");

    return tokens;
}

static char lexem[80] = "";

struct Token getNextToken(char *line)
{
    static char *m_line = 0;
    static int lastPos;
    static int beginline;

    if(line != NULL) {
        m_line = line;
        lastPos = 0;
        beginline = 1;
    }

    struct Token t;
    char *p = lexem;
    *p = '\0';

    while((m_line[lastPos] != '\0') && isspace(m_line[lastPos])) {
        ++lastPos;  /* skip whitespaces */
        printf("i'm here!\n");
    }

    if(isdigit(m_line[lastPos])) {  /* digit */
        while(isdigit(m_line[lastPos])) {
            *p = m_line[lastPos];
            ++lastPos;
            ++p;
        }
        *p = '\0';

        if(beginline) {
            t.type = LINE_NUMBER;
            beginline = 0;
        } else {
            t.type = CONSTANT;
        }
    } else if(isalpha(m_line[lastPos])) {   /* var or keyword */
        while(isalnum(m_line[lastPos])) {
            *p = m_line[lastPos];
            ++lastPos;
            ++p;
        }
        *p = '\0';

        int type = isKeyword(lexem);

        if(type == COMMENT) {
            t.lexem = NULL;
            return t;
        }

        if(type) {
            t.type = (TokenType) type;
        } else {
            t.type = VARIABLE;
        }
    } else if(m_line[lastPos] == '=') {
        if(m_line[lastPos + 1] == '=') {   /* double equals */
            lastPos += 2;
            *p = '='; ++p; *p = '='; ++p; *p = '\0';
            t.type = COMPARE;
        } else {
            lastPos += 1;
            *p = '='; ++p; *p = '\0';
            t.type = EQUALS;
        }
        printf("inside equal... \n");
    } else if(m_line[lastPos] == '!') {
        if(m_line[++lastPos] == '=') {
            *p = '!'; ++p; *p = '='; ++p; *p = '\0';
            t.type = COMPARE;
            ++lastPos;
        } else {
            ;   /* compilation error */
        }
    } else if(m_line[lastPos] == '<' || m_line[lastPos] == '>') {
        if(m_line[lastPos + 1] == '=') {
            *p = m_line[lastPos]; ++p;
            *p = '=';
            lastPos += 2;
        } else {
            *p = m_line[lastPos++];
        }

        ++p; *p = '\0';
        t.type = COMPARE;
    } else if(m_line[lastPos] == '-' || m_line[lastPos] == '+' ||
              m_line[lastPos] == '*' || m_line[lastPos] == '/') {
        *p = m_line[lastPos++]; ++p; *p = '\0';
        t.type = ARITHMETIC;
    } else if(m_line[lastPos] == '(' || m_line[lastPos] == ')') {
        *p = m_line[lastPos++]; ++p; *p = '\0';
        t.type = BRACE;
    } else {
        printf("compilation error: %c (%d)\n", m_line[lastPos], lastPos);
    }

    int len = strlen(lexem) + 1;
    if(len == 1) {  /* empty string */
/*        printf("lexem: %s\n", lexem); */
        printf("NULL lexem!\n");
        t.lexem = NULL;
    } else {
        char *l = (char*)malloc(sizeof(char) * len);
        memcpy(l, lexem, len);
        t.lexem = l;
    }

    return t;
}

int isKeyword(char *key)
{
    if(!strcmp(key, "rem"))
        return (int) COMMENT;
    if(!strcmp(key, "input"))
        return (int) INPUT;
    if(!strcmp(key, "print"))
        return (int) PRINT;
    if(!strcmp(key, "if"))
        return (int) IFGOTO;
    if(!strcmp(key, "goto"))
        return (int) GOTO;
    if(!strcmp(key, "let"))
        return (int) LET;
    if(!strcmp(key, "end"))
        return (int) END;

    return 0;
}

struct Tokens *addToken(struct Tokens *ts, struct Token token)
{
    if(ts->allocated == ts->size) {
        struct Token *tmp = (struct Token *)malloc(sizeof(struct Token) * ts->allocated * 1.5);
        memcpy(tmp, ts->tokens, sizeof(struct Token) * ts->size);
        free(ts->tokens);
        ts->tokens = tmp;
        ts->allocated *= 1.5;
        printf("realloc for tokens: allocated %d\n", ts->allocated);
    }

    ts->tokens[ts->size] = token;
    ts->size++;
    printf("tokens size now: %d\n", ts->size);
    return ts;
}

void freeTokens(struct Tokens *ts)
{
    printf("freeTokens: \n");
    int i = 0;
    printf("free every tokens...\n");
    while(i < ts->allocated) {
        printf("i: %d - lexem: %s\n", i, ts->tokens[i].lexem);
        free(ts->tokens[i].lexem);
        ++i;
    }

    printf("free ts\n");
    free(ts->tokens);
    free(ts);
    ts = 0;
    printf("... end\n");
}
