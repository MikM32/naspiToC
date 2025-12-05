
#include "lexer.h"


//-----------Definiciones--------------


//Constructor
void initMadaLexer(MadaLexer* self)
{
    self->source = NULL; // puntero original
    self->cur_source = NULL; // puntero sobre la cual se itera
    self->current_token.type = TOKEN_NONE;
    self->previous_token.type = TOKEN_NONE;
    self->line = self->col = 1;
    self->eol_flag = 0;

}

//Destructor
void destroyMadaLexer(MadaLexer* self)
{
    if(self->source != NULL)
    {
        free(self->source);
        initMadaLexer(self);
    }
}

void loadMadaSource(MadaLexer* self, char* source)
{
    self->source = source;
    self->cur_source = source;

}

static bool isAlpha(char c)
{
    if((c >= 'a' && c <= 'z') || (c >= 'A' && c<= 'Z'))
    {
        return true;
    }

    return false;
}

static bool isNum(char c)
{
    if( c >= '0' && c <= '9')
    {
        return true;
    }

    return false;
}

TokenType checkWord(int start, int length_key, int length_text, char* text, const char* keyword, TokenType type)
{
    //El motivo de por que use memcmp en lugar de strncmp es porque memcmp sigue comparando los bytes aunque estos sean nulos,
    // mientras que strncmp deja de comparar cuando llega a un byte nulo (Lo que podria llevar a resultados incorrectos).
    // Ejem: str1=holahola y str2=hola, strncmp "diria" que ambas cadenas son iguales y eso es incorrecto.

    if((length_key == length_text-start) && !memcmp(text+start, keyword, length_text-start))
    {
        return type;
    }

    return TOKEN_ID;
}

/* ======= PEQUEÑA OPTIMIZACIÓN ========
// A pesar de verse horrible XD, esta funcion evita
    comparar cadenas que no se acercan nisiquiera a las palabras clave.
    como, por ejemplo, "koala". La inicial k no coincide con la inicial
    de ninguna palabra clave por lo que no se llama a checkWord de forma
    innecesaria.


*/
TokenType isKeyword(char* text, int length)
{

    TokenType res = TOKEN_ID;

    switch(*text)
    {
        case 'a':{ res = checkWord(1, 8, length, text, "lgoritmo", TOKEN_ALG); break;}
        case 'e':{
            if(length >= 3)
            {
                switch(*(text+1))
                {
                    //case 'n': {res = checkWord(2, 4, length,text, "tero", TOKEN_INT_TYPE); break;}
                    case 's': {res = checkWord(2, 6, length,text, "cribir", TOKEN_PRINT); break;}
                }
            }
            break;
        }
        case 'f':{
            if(length >= 3)
            {
                switch(*(text+1))
                {
                    case 'a': {res = checkWord(2, 3, length, text, "lso", TOKEN_FALSE); break;}
                    case 'i': {res = checkWord(2, 1, length, text, "n", TOKEN_END); break;}
                    //case 'p': {res = checkWord(2, 3, length, text, "ara", TOKEN_FPARA); break;}
                    case 'u': {res = checkWord(2, 2, length, text, "nc", TOKEN_FUNC); break;}
                }
            }
            break;
        }
        case 'h':{
            if(length >= 3)
            {
                switch(*(text+1))
                {
                    case 'a':
                        {
                            if(length >= 4)
                            {
                                switch(*(text+2))
                                {
                                    case 'c': {res = checkWord(3, 2, length, text, "er", TOKEN_DO); break;}
                                    case 's': {res = checkWord(3, 2, length, text, "ta", TOKEN_UNTIL); break;}
                                }
                            }
                            break;
                        }

                }
            }
            break;
        }
        case 'i': {res = checkWord(1, 5, length, text, "nicio", TOKEN_BEGIN); break;}
        case 'p': {
            if(length >= 3)
            {
                switch(*(text+1))
                {
                    case 'a': {res = checkWord(2, 2, length, text, "ra", TOKEN_FOR); break;}
                    case 'r': {res = checkWord(2, 2, length, text, "oc", TOKEN_PROC); break;}
                }
            }
            break;

            }
        case 'v':{
            if(length >= 3)
            {
                switch(*(text+1))
                {
                    case 'a': {res = checkWord(2, 1, length, text, "r", TOKEN_VAR); break;}
                    case 'e': {res = checkWord(2, 7, length, text, "rdadero", TOKEN_TRUE); break;}
                }
            }
            break;
        }
        case 'o':{
            res = checkWord(1, 1, length, text, "r", TOKEN_OR);
            break;
        }
    }

    return res;
}

//Funcion que itera el codigo fuente y va obteniendo cada token hasta llegar al final del archivo (EOF)


void newLine(MadaLexer* self)
{
    self->line++;
    self->col=1;
}

void skipWhitespace(MadaLexer* self)
{
    while(1)
    {
        switch(*self->cur_source)
        {
            case ' ':
            case '\t':
            case '\r':
                self->cur_source++;
                self->col++;
                break;
            case '\n': //Si la bandera de EOL no esta puesta entonces se omiten todos los saltos de linea

                if(self->eol_flag) return;
                newLine(self);
                self->cur_source++;
                break;

            default:
                return;

        }
    }
}

void lexerNext(MadaLexer* self)
{
    if(self->source == NULL)
    {
        madaMessage("Lexer: No se ha cargado un codigo fuente que analizar.");
        return;
    }

    int cur_length=0; // Contador que registra la longitud de un lexema


    self->previous_token = self->current_token;
    skipWhitespace(self);



        if(*self->cur_source == '\n')
        {
            initMadaToken(&self->current_token, TOKEN_EOL, 1, self->cur_source, self->line, self->col);
            newLine(self);
            self->cur_source++;
            self->eol_flag=0;
        }
        else if(isAlpha(*self->cur_source))
        {

            //char buffer[256]= {0};
            //int i=0;

            char* sourcePos = self->cur_source;
            int curCol = self->col;

            while(isAlpha(*self->cur_source) || isNum(*self->cur_source))
            {
                //buffer[i] = *self->cur_source; //Posible bug (si el identificador sobrepasa los 255 caracteres no se maneja excepcion alguna)
                //i++;
                cur_length++;
                self->cur_source++;
                self->col++;
            }

            TokenType kType = isKeyword(sourcePos, cur_length);
            initMadaToken(&self->current_token, kType, cur_length, sourcePos, self->line, curCol);


            self->eol_flag=1;
        }
        else if(isNum(*self->cur_source))
        {
            //char buffer[256]= {0};
            //int i=0;

            char* sourcePos = self->cur_source;
            int curCol = self->col;

            while(isNum(*self->cur_source))
            {
                //buffer[i] = *self->cur_source; //Posible bug (si el identificador sobrepasa los 255 caracteres no se maneja excepcion alguna)
                //i++;
                cur_length++;
                self->cur_source++;
                self->col++;
            }


            if((*self->cur_source) == '.')
            {
                //buffer[i] = *self->cur_source;
                //i++;
                cur_length++;
                self->cur_source++;
                self->col++;
                while(isNum(*self->cur_source))
                {
                    //buffer[i] = *self->cur_source;
                    //i++;
                    cur_length++;
                    self->cur_source++;
                    self->col++;
                }
                initMadaToken(&self->current_token, TOKEN_REAL, cur_length, sourcePos, self->line, curCol);
            }
            else
            {
                initMadaToken(&self->current_token, TOKEN_INTEGER, cur_length, sourcePos, self->line, curCol);
            }

            self->eol_flag=1;
        }
        else if(*self->cur_source == '+')
        {
            initMadaToken(&self->current_token, TOKEN_PLUS, 1, self->cur_source, self->line, self->col);
            self->cur_source++;
            self->col++;
            self->eol_flag=1;
        }
        else if(*self->cur_source == '-')
        {
            if(*(self->cur_source+1) == '>')
            {
                initMadaToken(&self->current_token, TOKEN_ASSIGN, 2, self->cur_source, self->line, self->col);
                self->cur_source+=2;
                self->col+=2;
            }
            else
            {
                initMadaToken(&self->current_token, TOKEN_MINUS, 1, self->cur_source, self->line, self->col);
                self->cur_source++;
                self->col++;
            }

            self->eol_flag=1;
        }
        else if(*self->cur_source == '*')
        {
            initMadaToken(&self->current_token, TOKEN_MUL, 1, self->cur_source, self->line, self->col);
            self->cur_source++;
            self->col++;
            self->eol_flag=1;
        }
        else if(*self->cur_source == '/')
        {
            initMadaToken(&self->current_token, TOKEN_DIV, 1, self->cur_source, self->line, self->col);
            self->cur_source++;
            self->col++;
            self->eol_flag=1;
        }
        else if(*self->cur_source == '=')
        {
            initMadaToken(&self->current_token, TOKEN_EQU, 1, self->cur_source, self->line, self->col);
            self->cur_source++;
            self->col++;
            self->eol_flag=1;
        }
        else if(*self->cur_source == '<')
        {
            if(*(self->cur_source+1) == '=' )
            {
                initMadaToken(&self->current_token, TOKEN_LESS_EQ, 2, self->cur_source, self->line, self->col);
                self->cur_source+=2;
                self->col+=2;
            }
            else
            {
                initMadaToken(&self->current_token, TOKEN_LESS, 1, self->cur_source, self->line, self->col);
                self->cur_source++;
                self->col++;
            }

            self->eol_flag=1;
        }
        else if(*self->cur_source == '>')
        {
            if(*(self->cur_source+1) == '=' )
            {
                initMadaToken(&self->current_token, TOKEN_BIGGER_EQ, 2, self->cur_source, self->line, self->col);
                self->cur_source+=2;
                self->col+=2;
            }
            else
            {
                initMadaToken(&self->current_token, TOKEN_BIGGER, 1, self->cur_source, self->line, self->col);
                self->cur_source++;
                self->col++;
            }

            self->eol_flag=1;
        }
        else if(*self->cur_source == '(')
        {
            initMadaToken(&self->current_token, TOKEN_OPAREN, 1, self->cur_source, self->line, self->col);
            self->cur_source++;
            self->col++;
            self->eol_flag=1;
        }
        else if(*self->cur_source == ')')
        {
            initMadaToken(&self->current_token, TOKEN_CPAREN, 1, self->cur_source, self->line, self->col);
            self->cur_source++;
            self->col++;
            self->eol_flag=1;
        }
        else if(*self->cur_source == ',')
        {
            initMadaToken(&self->current_token, TOKEN_COMMA, 1, self->cur_source, self->line, self->col);
            self->cur_source++;
            self->col++;
            self->eol_flag=1;
        }
        else if(*self->cur_source == ':')
        {
            initMadaToken(&self->current_token, TOKEN_COLON, 1, self->cur_source, self->line, self->col);
            self->cur_source++;
            self->col++;
            self->eol_flag=1;
        }
        else if(*self->cur_source == '[')
        {
            initMadaToken(&self->current_token, TOKEN_OBRACKET, 1, self->cur_source, self->line, self->col);
            self->cur_source++;
            self->col++;
            self->eol_flag=1;
        }
        else if(*self->cur_source == ']')
        {
            initMadaToken(&self->current_token, TOKEN_CBRACKET, 1, self->cur_source, self->line, self->col);
            self->cur_source++;
            self->col++;
            self->eol_flag=1;
        }
        else if(*self->cur_source == '"')
        {
            int col = self->col;
            self->cur_source++;
            self->col++;

            char* str_source = self->cur_source;
            //int i=0;

            while(*self->cur_source != '"' && *self->cur_source != '\0')
            {
                cur_length++;
                self->cur_source++;
                self->col++;
            }

            initMadaToken(&self->current_token, TOKEN_STRING, cur_length, str_source, self->line, col);
            self->cur_source++;
            self->col++;
            self->eol_flag=1;
        }
        else if(*self->cur_source == '\0')
        {
            initMadaToken(&self->current_token, TOKEN_EOF, 1, self->cur_source, self->line, self->col);
            //Llego al final del codigo fuente por lo que deja de iterar (no se hace self->cur_source++)
        }
        else // Caracter no soportado. Ejem: ñ
        {
            //invalidCharError(self, *self->cur_source);
            initMadaTokenError(&self->current_token, "Caracter invalido", self->line, self->col);
            self->cur_source++;
            self->col++;
            self->eol_flag=1;

        }

        //cur_length = 0; //reinicia la longitud del lexema

        #ifdef DEBUG_LEXER_MODE
        madaMessage("Lexer debug");
        printf("Current: ");
        printToken(self->current_token);
        printf("Previous: ");
        printToken(self->previous_token);
        #endif // DEBUG_MODE

    //}

}
