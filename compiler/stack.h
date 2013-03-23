#ifndef STACK_H
#define STACK_H

#include "parser.h"

struct Stack {
    struct Token *top;
    int size;
    int allocated;
};

struct Stack *init(struct Stack *s);
void clearStack(struct Stack *s);
void pushIntoStack(struct Stack *s, struct Token token);
struct Token popFromStack(struct Stack *s);
struct Token topStack(struct Stack *s);

#endif /* STACK_H */
