#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

#include "lexer.h"
#include "Ast.h"

#define EXPECT_EOL consume(parser, TOKEN_EOL, "Se esperaba un salto de linea")

typedef enum
{
    PREC_NONE,
    PREC_ASSIGNMENT,
    PREC_OR,
    PREC_AND,
    PREC_EQUALITY,
    PREC_COMPARISON,
    PREC_TERM,
    PREC_FACTOR,
    PREC_UNARY,
    PREC_CALL,
    PREC_PRIMARY
}ParserPrecedence;


typedef struct MadaParser
{
    MadaLexer lexer;
    AstNode* ast;
    bool panicMode;
    bool hadError;
}MadaParser;

typedef AstNode*(*PrefixFunc)(MadaParser*);
typedef AstNode*(*InfixFunc)(MadaParser*, AstNode* left);

typedef struct ParserRule
{
    PrefixFunc prefix;
    InfixFunc infix;
    ParserPrecedence precedence;
}ParserRule;

AstNode* parseInteger(MadaParser* parser);
AstNode* parseReal(MadaParser* parser);
AstNode* parseTrue(MadaParser* parser);
AstNode* parseFalse(MadaParser* parser);
AstNode* parseString(MadaParser* parser);
AstNode* parseCall(MadaParser* parser, AstNode* left);
AstNode* parseVariable(MadaParser* parser);
AstNode* parseUnary(MadaParser* parser);
AstNode* parseBinary(MadaParser* parser, AstNode* left);
AstNode* parseGrouping(MadaParser* parser);

AstNode* statement(MadaParser* parser);
AstNode* block(MadaParser* parser);


#endif // PARSER_H_INCLUDED
