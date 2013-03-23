#include <stdlib.h>
#include "stack.h"

struct Stack *init(struct Stack *s)
{
    s = (struct Stack *)malloc(sizeof(struct Stack));
    s->top = (struct Token*)malloc(sizeof(struct Token) * 10);
    s->size = 0;
    s->allocated = 10;
    return s;
}

void clearStack(struct Stack *s)
{
    free(s->top);
    free(s);
}

void pushIntoStack(struct Stack *s, struct Token token)
{
    if(s->size == s->allocated) {
        s->top = (struct Token*)realloc(s->top, sizeof(struct Token) * s->size * 1.5);
        s->allocated *= 1.5;
    }

    *(s->top + s->size) = token;
    ++s->size;
}

struct Token popFromStack(struct Stack *s)
{
    if(s->size > 0) {
/*        struct Token tmp = *(s->top + s->size); */
        --s->size;
        return *(s->top + s->size);
    } /* else { } error */

    struct Token null;
    null.lexem = NULL;
    return null;
}

struct Token topStack(struct Stack *s)
{
    if(s->size > 0) {
        return *(s->top + s->size - 1);
    }

    struct Token tmp;
    tmp.lexem = NULL;
    return tmp;
}

