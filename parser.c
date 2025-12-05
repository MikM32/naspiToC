#include "parser.h"

ParserRule parseTable[] =
{
    [TOKEN_OPAREN]  = {parseGrouping,parseCall,  PREC_PRIMARY},
    [TOKEN_OBRACKET] = {NULL, NULL/*Parse Subscript*/, PREC_PRIMARY},
    [TOKEN_INTEGER] = {parseInteger, NULL,  PREC_PRIMARY},
    [TOKEN_REAL]    = {parseReal,    NULL,  PREC_PRIMARY},
    [TOKEN_TRUE]    = {parseTrue,   NULL,   PREC_PRIMARY},
    [TOKEN_FALSE]   = {parseFalse,   NULL,  PREC_PRIMARY},
    [TOKEN_STRING]   = {parseString,   NULL,  PREC_PRIMARY},
    [TOKEN_ID]      = {parseVariable,NULL,  PREC_NONE},
    [TOKEN_MUL]     = {NULL,    parseBinary, PREC_FACTOR},
    [TOKEN_DIV]     = {NULL,    parseBinary, PREC_FACTOR},
    [TOKEN_PLUS]    = {NULL,    parseBinary, PREC_TERM},
    [TOKEN_MINUS]   = {parseUnary,    parseBinary, PREC_TERM},
    [TOKEN_LESS]    = {NULL,    parseBinary, PREC_COMPARISON},
    [TOKEN_LESS_EQ] = {NULL,    parseBinary, PREC_COMPARISON},
    [TOKEN_BIGGER]  = {NULL,    parseBinary, PREC_COMPARISON},
    [TOKEN_BIGGER_EQ]= {NULL,    parseBinary, PREC_COMPARISON},
    [TOKEN_AND]      = {NULL, parseBinary, PREC_AND},
    [TOKEN_OR]      = {NULL, parseBinary, PREC_OR},
    [TOKEN_CPAREN]  = {NULL, NULL, PREC_NONE},
    [TOKEN_EOL]     = {NULL, NULL, PREC_NONE},
    [TOKEN_ERROR]  = {NULL, NULL, PREC_NONE},
    [TOKEN_NONE]    = {NULL, NULL, PREC_NONE}
};



void parseNext(MadaParser* parser)
{
    lexerNext(&parser->lexer);
}

void initMadaParser(MadaParser* parser)
{
    parser->ast = NULL;
    parser->panicMode = false;
    parser->hadError = false;
    initMadaLexer(&parser->lexer);

    //parseNext(parser);
}

ParserRule* getRule(TokenType type)
{
    return &parseTable[type];
}

void errorAt(MadaParser* parser, MadaToken token, char* msg)
{
    if(parser->panicMode) return;

    parser->panicMode = true;

    fprintf(stderr, "[linea %d][Col %d] Error de sintaxis", token.line_num, token.col_num);

    switch(token.type)
    {

        case TOKEN_EOF:
            fprintf(stderr, " al final del archivo");
            break;
        case TOKEN_ERROR:

            fprintf(stderr, " (Lexico: %.*s)", token.length, token.in_source_pos);
            break;

        default:
            fprintf(stderr, " en '%.*s'", token.length, token.in_source_pos);
            break;
    }

    fprintf(stderr, ": %s.\n", msg);
    parser->hadError = true;
}

void errorAtCurrent(MadaParser* parser, char* msg)
{
    errorAt(parser, parser->lexer.current_token, msg);
}

void errorAtPrevious(MadaParser* parser, char* msg)
{
    errorAt(parser, parser->lexer.previous_token, msg);
}

bool check(MadaParser* parser, TokenType type)
{
    if(parser->lexer.current_token.type == type)
    {
        return true;
    }

    return false;
}

bool match(MadaParser* parser, TokenType type)
{
    if(check(parser, type))
    {
        parseNext(parser);
        return true;
    }

    return false;
}

void consume(MadaParser* parser, TokenType type, char* error_msg)
{
    if(parser->lexer.current_token.type != type)
    {
        errorAtCurrent(parser, error_msg);
    }

    parseNext(parser);
}



AstNode* parsePrecedence(MadaParser* parser, ParserPrecedence precedence)
{
    parseNext(parser);
    PrefixFunc prefixFunc = getRule(parser->lexer.previous_token.type)->prefix;
    if(prefixFunc==NULL)
    {
        errorAtCurrent(parser, "Se esperaba una expresion");
        return NULL;
    }

    AstNode* left = prefixFunc(parser);

    while(precedence <= getRule(parser->lexer.current_token.type)->precedence)
    {
        parseNext(parser);
        InfixFunc infixFunc = getRule(parser->lexer.previous_token.type)->infix;
        if(infixFunc==NULL)
        {
            errorAtPrevious(parser, "Se esperaba un operador en la expresion");
            return NULL;
        }
        left = infixFunc(parser, left);
    }

    return left;
}

AstNode* parseInteger(MadaParser* parser)
{
    AstNode* entero = newAstValue(parser->lexer.previous_token, AST_INTEGER);
    //parseNext(parser);
    return entero;
}

AstNode* parseReal(MadaParser* parser)
{
    AstNode* real = newAstValue(parser->lexer.previous_token, AST_REAL);
    //parseNext(parser);
    return real;
}

AstNode* parseString(MadaParser* parser)
{
    AstNode* string = newAstValue(parser->lexer.previous_token, AST_STR_LIT);
    //parseNext(parser);
    return string;
}

AstNode* parseTrue(MadaParser* parser)
{
    AstNode* real = newAstValue(parser->lexer.previous_token, AST_TRUE);
    //parseNext(parser);
    return real;
}

AstNode* parseFalse(MadaParser* parser)
{
    AstNode* real = newAstValue(parser->lexer.previous_token, AST_FALSE);
    //parseNext(parser);
    return real;
}

AstNode* parseCall(MadaParser* parser, AstNode* left)
{
    return NULL;
}

AstNode* parseVariable(MadaParser* parser)
{
    AstNode* var = newAstVar(parser->lexer.previous_token);

    return var;
}

AstNode* parseUnary(MadaParser* parser)
{
    MadaToken operator = parser->lexer.previous_token;
    ParserRule* rule = getRule(operator.type);

    AstNode* right = parsePrecedence(parser, rule->precedence+1);

    return newAstUnop(right, operator);
}

AstNode* parseBinary(MadaParser* parser, AstNode* left)
{
    MadaToken operator = parser->lexer.previous_token;
    ParserRule* rule = getRule(operator.type);

    AstNode* right = parsePrecedence(parser, rule->precedence+1); //le suma 1 a la precedencia para ir a la mayor.

    return newAstBinop(left, right, operator);
}

AstNode* expression(MadaParser* parser)
{
    return parsePrecedence(parser, PREC_ASSIGNMENT);
}

AstNode* parseGrouping(MadaParser* parser)
{
    AstNode* node = expression(parser);
    consume(parser, TOKEN_CPAREN, "Se esperaba un \")\"");

    return node;
}

AstNode* expressionStatement(MadaParser* parser)
{
    AstNode* node = expression(parser);
    EXPECT_EOL;

    return node;
}

void synchronize(MadaParser* parser)
{
    parser->panicMode = false;
    while (parser->lexer.current_token.type != TOKEN_EOF) {
    if (parser->lexer.previous_token.type == TOKEN_EOL) return;
    switch (parser->lexer.current_token.type) {
      case TOKEN_FUNC:
      case TOKEN_FOR:
      case TOKEN_IF:
      case TOKEN_WHILE:
      case TOKEN_PRINT:
      case TOKEN_RETURN:
        return;

      default:
        ; // Do nothing.
    }

      parseNext(parser);
  }
}

AstNode* varAssignment(MadaParser* parser)
{
    AstNode* var = parseVariable(parser);
    consume(parser, TOKEN_ASSIGN, "Se esperaba \"->\"");

    AstNode* res = newAstVarAssign(var, expression(parser));

    EXPECT_EOL;
    return res;
}

AstNode* varDeclaration(MadaParser* parser)
{
    AstNode* vardecl = NULL;

    MadaToken type = parser->lexer.previous_token;

    if(match(parser, TOKEN_COLON))
    {
        consume(parser, TOKEN_ID, "Se esperaba un identificador");
        vardecl = newAstVarDecl(parser->lexer.previous_token, type);
    }


    return vardecl;
}

AstNode* argStmt(MadaParser* parser, bool isDeclaration)
{
    AstNode* res = NULL, *acum=NULL;

    while(!check(parser, TOKEN_CPAREN) && !check(parser, TOKEN_EOF))
    {
        if(!acum)
        {
            if(!isDeclaration) res = newAstArgs(expression(parser), NULL);
            else
            {
                match(parser, TOKEN_ID);
                res = newAstArgs(varDeclaration(parser), NULL);
            }
            acum = res;
        }
        else
        {
            if(!isDeclaration) acum->AST_ARGS.next = newAstArgs(expression(parser), NULL);
            else{
                match(parser, TOKEN_ID);
                    acum->AST_ARGS.next = newAstArgs(varDeclaration(parser), NULL);
                    }
            acum = acum->AST_ARGS.next;
        }

        if(!match(parser, TOKEN_COMMA))
        {
            break;
        }
    }

    return res;
}

AstNode* procedureCall(MadaParser* parser)
{
    AstNode* res = NULL;

    MadaToken procName = parser->lexer.previous_token;

    consume(parser, TOKEN_OPAREN, "Se esperaba un \"(\"");
    AstNode* args = argStmt(parser, false);
    consume(parser, TOKEN_CPAREN, "Se esperaba un \")\"");

    res = newAstProcCall(procName, args);

    EXPECT_EOL;

    return res;
}

AstNode* procedureDecl(MadaParser* parser)
{
    AstNode* res = NULL;
    AstNode* procBlock = NULL;

    consume(parser, TOKEN_ID, "Se esperaba el identificador del procedimiento");

    MadaToken procName = parser->lexer.previous_token;

    consume(parser, TOKEN_OPAREN, "Se esperaba \"(\"");
    AstNode* args = argStmt(parser, true);
    consume(parser, TOKEN_CPAREN, "Se esperaba \")\"");
    EXPECT_EOL;

    if(match(parser, TOKEN_BEGIN))
    {
        EXPECT_EOL;
        procBlock = block(parser);
    }

    res = newAstProcDecl(procName, args, procBlock, AST_NONE);

    return res;

}

AstNode* forStmt(MadaParser* parser)
{
    AstNode* res = NULL;
    AstNode* iterator = NULL;
    AstNode* condition = NULL;
    AstNode* statements = NULL;

    consume(parser, TOKEN_ID, "Se esperaba una variable");

    AstNode* var = parseVariable(parser);

    if(match(parser, TOKEN_ASSIGN))
    {
        AstNode* expr = expression(parser);
        iterator = newAstVarAssign(var, expr);
    }
    else
    {
        iterator = var;
    }

    consume(parser, TOKEN_UNTIL, "Se esperaba \"hasta\"");

    condition = expression(parser);

    consume(parser, TOKEN_DO, "Se esperaba \"hacer\"");
    EXPECT_EOL;

    statements = block(parser);

    res = newAstFor(iterator, condition, statements);
    return res;

}

AstNode* statement(MadaParser* parser)
{
    AstNode* res = NULL;
    if(match(parser, TOKEN_ID))
    {
        if(check(parser, TOKEN_COLON))
        {
            res = varDeclaration(parser);
            EXPECT_EOL;
        }
        else if(check(parser, TOKEN_OPAREN))
        {
            res = procedureCall(parser);
            //EXPECT_EOL;
        }
        else res = varAssignment(parser);
    }
    /*else if(match(parser, TOKEN_BEGIN)) // Sep, los bloques de codigo son sentencias tambien XD.
    {
        EXPECT_EOL;
        res = block(parser);
    }*/
    else if(match(parser, TOKEN_FOR))
    {
        res = forStmt(parser);
    }
    else if(match(parser, TOKEN_PRINT))
    {
        res = procedureCall(parser);
    }

    if(parser->panicMode) synchronize(parser);

    res = newAstStmt(res);

    return res;
}

AstNode* block(MadaParser* parser)
{
    AstNode* res = NULL, *acum=NULL;
    while(!check(parser, TOKEN_END) && !check(parser, TOKEN_EOF))
    {
        if(!acum)
        {
            res = newAstCompoundStmt(statement(parser), NULL);
            acum = res;
        }
        else
        {
            acum->AST_COMPOUND_STMT.next = newAstCompoundStmt(statement(parser), NULL);
            acum = acum->AST_COMPOUND_STMT.next;
        }
    }

    res = newAstBlock(res);

    consume(parser, TOKEN_END, "Se esperaba el fin de bloque \"fin\"");
    EXPECT_EOL;

    return res;
}

AstNode* algorithm(MadaParser* parser)
{
    AstNode* res = NULL;
    consume(parser, TOKEN_ALG, "Se esperaba la palabra clave \"algoritmo\"");
    consume(parser, TOKEN_ID, "El algoritmo debe tener un nombre");

    MadaToken algName = parser->lexer.previous_token;

    EXPECT_EOL;

    AstNode* procDecl= NULL, *proc = NULL, *acum = NULL;
    AstNode* algBlock = NULL;

    while(!check(parser, TOKEN_BEGIN) && !check(parser, TOKEN_EOF))
    {
        if(match(parser, TOKEN_PROC))
        {
            proc = procedureDecl(parser);
        }
        else if(match(parser, TOKEN_FUNC))
        {

        }


        if(!acum)
        {
            procDecl = newAstCompoundStmt(proc, NULL);
            acum = procDecl;
        }
        else
        {
            acum->AST_COMPOUND_STMT.next = newAstCompoundStmt(proc, NULL);
            acum = acum->AST_COMPOUND_STMT.next;
        }
    }



    if(match(parser, TOKEN_BEGIN))
    {
        EXPECT_EOL;
        algBlock = block(parser);
    }

    res = newAstAlg(algName, procDecl, algBlock);

    return res;

}









