#ifndef LEXER_H_INCLUDED
#define LEXER_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "utils.h"
#include "token.h"
//#define DEBUG_LEXER_MODE

typedef struct MadaLexer
{
    char* source; // puntero a la cadena del codigo.
    char* cur_source; // ubicacion actual en la cadena del codigo.
    MadaToken current_token;
    MadaToken previous_token;
    int line, col;
    bool eol_flag;

}MadaLexer;

void initMadaLexer(MadaLexer* self);
void destroyMadaLexer(MadaLexer* self);
void loadMadaSource(MadaLexer* self, char* source);

#endif // LEXER_H_INCLUDED
