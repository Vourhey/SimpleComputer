/* Simpletron emulator
 * Discription in C (or C++): How to programm - Harvey M. Deitle */

/* Simpletron is decemal computer. It has 100 cells of memory and accumulator.
 * Word (one cell) can hold data or instruction. Word's range from -9999 to +9999. */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define VERSION "0.1"

struct Simpletron {
    int memory[100];
    int accumulator;
    int instructionCounter;
    int instructionRegister;
    int operationCode;
    int operand;
};

void help(const char *name);
void printDump(struct Simpletron *s);
int checkRange(int n);
void printError(const char *str);

/* Instruction set */
#define READ        10
#define WRITE       11
#define LOAD        20
#define STORE       21
#define ADD         30
#define SUBSTRACT   31
#define DIVIDE      32
#define MULTIPLY    33
#define BRANCH      40
#define BRANCHNEG   41
#define BRANCHZERO  42
#define HALT        4300

int main(int argc, char **argv)
{
    struct Simpletron *s = (struct Simpletron *)calloc(1, sizeof(struct Simpletron));

    int dump = 0;   /* show dump or not */
    char *fileName;

    /* terribly... */
    if(argc == 3) {
        if(!strcmp("--dump", argv[2])) {
            dump = 1;
        }
        fileName = argv[1];
    } else if(argc == 2) {
        if(!strcmp(argv[1], "--help") || !strcmp(argv[1], "-h")) {
            help(argv[0]);
            return 0;
        } else {
            fileName = argv[1];
        }
    } else {
        printf("Incorrect input format\n");
        help(argv[0]);
        return 1;
    }

    FILE *in = fopen(fileName, "rb");
    if(in == NULL) {
        printf("Can't open file %s", fileName);
        return 1;
    }

    while(fread(&s->instructionRegister, sizeof(int), 1, in)) {
        s->memory[s->instructionCounter++] = s->instructionRegister;
    }
    
    s->instructionCounter  = 0;
    s->instructionRegister = s->memory[s->instructionCounter];

    while(s->instructionRegister != HALT && s->instructionCounter < 100) {
        ++s->instructionCounter;
        s->operationCode = s->instructionRegister / 100;
        s->operand = s->instructionRegister % 100;

        switch(s->operationCode) {
        case READ:
            scanf("%d", &s->memory[s->operand]);
            break;
        case WRITE:
            printf("%d\n", s->memory[s->operand]);
            break;
        case LOAD:
            s->accumulator = s->memory[s->operand];
            break;
        case STORE:
            s->memory[s->operand] = s->accumulator;
            break;
        case ADD:
            s->accumulator += s->memory[s->operand];
            if(!checkRange(s->accumulator)) {   /* maybe, it's have not to stop program...? */
                printError("overflow when add");
                printDump(s);
                return 0;
            }
            break;
        case SUBSTRACT:
            s->accumulator -= s->memory[s->operand];
            if(!checkRange(s->accumulator)) {
                printError("overflow when substract");
                printDump(s);
                return 0;
            }
            break;
        case DIVIDE:
            if(s->memory[s->operand] == 0) {
                printError("division by zero!");
                printDump(s);
                return 0;
            }
            s->accumulator /= s->memory[s->operand];
            break;
        case MULTIPLY:
            s->accumulator *= s->memory[s->operand];
            if(!checkRange(s->accumulator)) {
                printError("overflow when multiply");
                printDump(s);
                return 0;
            }
            break;
        case BRANCH:
            s->instructionCounter = s->operand;
            break;
        case BRANCHNEG:
            if(s->accumulator < 0) {
                s->instructionCounter = s->operand;
            }
            break;
        case BRANCHZERO:
            if(s->accumulator == 0) {
                s->instructionCounter = s->operand;
            }
            break;
        }
        
        s->instructionRegister = s->memory[s->instructionCounter];
    }

    if(dump) {  
        printDump(s);
    }

    free(s);

    return 0;
}

void help(const char *name)
{
    printf( "Simpletron emulator v%s\n"
            "Usage: %s \"input file\" [--dump][--help|-h]\n",
            VERSION, name);
}

void printDump(struct Simpletron *s)
{
    printf("\nRegisters:\n"
           "accumulator\t\t%+05d\n"
           "instructionCounter\t  %+03d\n"
           "instructionRegister\t%+05d\n"
           "operationCode\t\t  %+03d\n"
           "operand\t\t\t  %+03d\n\n", s->accumulator, s->instructionCounter,
                                      s->instructionRegister, s->operationCode,
                                      s->operand);

    printf("Memory:\n  ");

    int i = 0, j = 0;
    for(; i<10; ++i) {
        printf("     %d", i);
    }

    printf("\n");

    for(i=0; i<100; i += 10) {
        printf("%2d", i);

        for(j=0; j<10; ++j) {
            printf(" %+05d", s->memory[i + j]);
        }
        printf("\n");
    }
}

int checkRange(int n)
{
    if(n >= -9999 && n <= 9999) {
        return 1;
    }
    
    return 0;
}

void printError(const char *str)
{
    printf("Fatal error: %s\n", str);
}

