#include "token.h"

void initMadaToken(MadaToken* token, TokenType type,int length, char* raw_pos, int line_num, int col_num)
{
    token->type = type;
    token->line_num = line_num;
    token->col_num = col_num;
    token->length = length;
    token->in_source_pos = raw_pos;
}

void initMadaTokenError(MadaToken* token, char* error_msg, int line, int col)
{
    token->type = TOKEN_ERROR;
    token->in_source_pos = error_msg;
    token->length = strlen(error_msg);
    token->line_num = line;
    token->col_num = col;
}

void printToken(MadaToken token)
{
    //printf("Linea: %d | Col: %d | ", token.line_num, token.col_num);
    printf("TOKEN: ");

    printTokenString(token);

    printf("\n");
}

int testToken(MadaToken token, TokenType type)
{
    return (token.type == type);
}

void printTokenStringEx(MadaToken token, FILE* outbuf)
{
    switch(token.type)
    {
        case TOKEN_EOF:
            fprintf(outbuf, "EOF");
            break;
        case TOKEN_EOL:
            fprintf(outbuf, "EOL");
            break;
        default:
            {
                fprintf(outbuf, "%.*s", token.length, token.in_source_pos);
                break;
            }
    }
}

void printTokenString(MadaToken token)
{
    printTokenStringEx(token, stdout);
}

bool tokenEqual(MadaToken tok1, MadaToken tok2)
{
    if(tok1.in_source_pos = tok2.in_source_pos)
    {
        return true;
    }

    return false;
}

void printTokenStringLitEx(MadaToken token, FILE* outbuf)
{
    fprintf(outbuf, "\"");
    printTokenStringEx(token, outbuf);
    fprintf(outbuf, "\"");
}

void printTokenStringLit(MadaToken token)
{
    printTokenStringLitEx(token, stdout);
}

void destroyMadaToken(MadaToken* token)
{
    token->type = TOKEN_NONE;

    /*
    if(token->val != NULL)
    {
        free(token->val);
        token->val= NULL;
    }*/
}
