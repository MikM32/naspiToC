#ifndef TOKEN_H_INCLUDED
#define TOKEN_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef enum enum_toktype
{
    TOKEN_NONE=0,
    TOKEN_ID=1,
    TOKEN_INTEGER = 2,
    TOKEN_REAL,
    TOKEN_FALSE,
    TOKEN_TRUE,
    TOKEN_BOOL,
    TOKEN_STRING,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MUL,
    TOKEN_DIV,
    TOKEN_EQU,
    TOKEN_LESS,
    TOKEN_BIGGER,
    TOKEN_LESS_EQ,
    TOKEN_BIGGER_EQ,
    TOKEN_OR,
    TOKEN_AND,
    TOKEN_NOT,
    TOKEN_COLON, // Colon = dos puntos
    TOKEN_COMMA,
    TOKEN_OPAREN, // Open Parentesis
    TOKEN_CPAREN, // Close Parentesis
    TOKEN_OBRACKET, // Open Bracket
    TOKEN_CBRACKET, // Close bracket
    TOKEN_ASSIGN,
    TOKEN_PROC,
    TOKEN_REF,
    TOKEN_FUNC,
    TOKEN_ALG,
    TOKEN_VAR,
    TOKEN_FOR,
    TOKEN_UNTIL,
    TOKEN_DO,
    //TOKEN_FFOR,
    TOKEN_IF,
    //TOKEN_FIF,
    TOKEN_WHILE,
    TOKEN_INT_TYPE,
    TOKEN_REAL_TYPE,
    TOKEN_BEGIN,
    TOKEN_PRINT,
    TOKEN_RETURN,
    TOKEN_END,
    TOKEN_EOL,
    TOKEN_EOF,
    TOKEN_ERROR

}TokenType;

typedef struct MadaToken
{
    TokenType type;
    int length;
    char* in_source_pos; // apunta a la direccion del caracter que empieza en la cadena del token del codigo fuente original (o raw)
    int line_num;
    int col_num;
} MadaToken;

void initMadaToken(MadaToken* token, TokenType type, int length, char* raw_pos, int line_num, int col_num);
void initMadaTokenError(MadaToken* token, char* error_msg, int line, int col);
void printToken(MadaToken token);
int testToken(MadaToken token, TokenType type);
void printTokenString(MadaToken token);
void destroyMadaToken(MadaToken* token);


#endif // TOKEN_H_INCLUDED
