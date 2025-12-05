#ifndef SEM_H_INCLUDED
#define SEM_H_INCLUDED

#include "parser.h"
#define MAX_EXPR 255
#define MAX_ARGS 30

#define GLOBAL_SCOPE 0

typedef enum
{
    SYM_VAR,
    SYM_CALL,
}SymbolType;

typedef enum
{
    CALL_WRITE,
    CALL_READ,
    CALL_USERDEF,
}CallType;

typedef struct Variable
{
    MadaToken token;
    MadaToken procName;
    AstTag type;
}Variable;

typedef struct Callable
{
    MadaToken name;
    int arg_num;
    bool needTypeCheck;
    AstTag returnType;
    CallType type;

}Callable;


typedef struct Symbol
{
    SymbolType type;
    int scope;
    union
    {
        struct Variable variable;
        struct Callable callable;
    };
}Symbol;

typedef struct MadaSemantic
{
    MadaParser parser;
    HashTable symbolTable;
    HashTable typeTable;
    AstTag exprStack[MAX_EXPR]; //Pila para verificar si los tipos de una expresion coinciden.
    uint8_t exprStackIndex;
    int currentScope;
    bool argDecl;
    FILE* outputBuffer;
    MadaToken currentProcName;
}MadaSemantic;

void initMadaSemantic(MadaSemantic* sem);

#endif // SEM_H_INCLUDED
