#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>

typedef enum {
    LINE_NUMBER = 1,
    COMMENT,
    VARIABLE,
    CONSTANT,
    INPUT,
    PRINT,
    END,
    IFGOTO,
    LET,
    GOTO,
    COMPARE,
    EQUALS,
    ARITHMETIC,
    BRACE,
    OTHER   /* for errors */
} TokenType;

struct Token {
    char *lexem;
    TokenType type;
};

struct Tokens {
    struct Token *tokens;
    int size;
    int allocated;
};

extern struct Tokens *parse(FILE *in);
extern void freeTokens(struct Tokens *ts);

#endif /* PARSER_H */
