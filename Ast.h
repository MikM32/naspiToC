#ifndef AST_H_INCLUDED
#define AST_H_INCLUDED

#include "token.h"

typedef enum
{
    AST_NONE,
    AST_BINOP,
    AST_UNOP,
    AST_VALUE,
    AST_INTEGER,
    AST_REAL,
    AST_TRUE,
    AST_STR_LIT,
    AST_ALG,
    AST_FALSE,
    AST_BOOL,
    AST_VAR,
    AST_FOR,
    AST_WHILE,
    AST_VARASSIGN,
    AST_VAR_DECL,
    AST_ARGS,
    AST_PROC_CALL,
    AST_PROC_DECL,
    AST_BLOCK,
    AST_COMPOUND_STMT,
    AST_STMT,

}AstTag;

typedef struct AstNode
{
    AstTag tag;

    union
    {
        struct AST_BINOP{ struct AstNode* left, *right; MadaToken operator;} AST_BINOP;
        struct AST_UNOP{ struct AstNode* right; MadaToken operator;} AST_UNOP;
        struct AST_VALUE{ MadaToken value; AstTag vtype;} AST_VALUE;
        struct AST_VAR{ MadaToken var;} AST_VAR;
        struct AST_VARASSIGN{ struct AstNode* var; struct AstNode* expr;} AST_VARASSIGN;
        struct AST_FOR{ struct AstNode* iterator, *condition, *statements; } AST_FOR;
        struct AST_VAR_DECL{ MadaToken var, type;} AST_VAR_DECL;
        struct AST_PROC_CALL{ MadaToken procName; struct AstNode* args;} AST_PROC_CALL;
        struct AST_PROC_DECL{ MadaToken procName; struct AstNode* args, *procBlock; AstTag returnType} AST_PROC_DECL;
        struct AST_ARGS{ struct AstNode* arg, *next;} AST_ARGS;
        struct AST_BLOCK{ struct AstNode* compound_stmt;} AST_BLOCK;
        struct AST_STMT{ struct AstNode* stmt;} AST_STMT;
        struct AST_COMPOUND_STMT{ struct AstNode* statement, *next;} AST_COMPOUND_STMT;
        struct AST_ALG{ MadaToken id; struct AstNode* decls, *algBlock;} AST_ALG;
    };
} AstNode;

#endif // AST_H_INCLUDED
