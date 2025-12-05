
#include "memory.h"
#include "ast.h"

AstNode* newAstValue(MadaToken value, AstTag type)
{
    AstNode* newNode = MadAlloc(sizeof(AstNode));

    newNode->tag = AST_VALUE;
    newNode->AST_VALUE.value = value;
    newNode->AST_VALUE.vtype = type;

    return newNode;

}

AstNode* newAstVar(MadaToken var)
{
    AstNode* newNode = MadAlloc(sizeof(AstNode));

    newNode->tag = AST_VAR;
    newNode->AST_VAR.var = var;

    return newNode;
}

AstNode* newAstUnop(AstNode* right, MadaToken operator)
{
    AstNode* newNode = MadAlloc(sizeof(AstNode));

    newNode->tag = AST_UNOP;
    newNode->AST_UNOP.right = right;
    newNode->AST_UNOP.operator = operator;

    return newNode;
}

AstNode* newAstBinop(AstNode* left, AstNode* right, MadaToken operator)
{
    AstNode* newNode = MadAlloc(sizeof(AstNode));

    newNode->tag = AST_BINOP;
    newNode->AST_BINOP.left = left;
    newNode->AST_BINOP.right = right;
    newNode->AST_BINOP.operator = operator;

    return newNode;
}

AstNode* newAstVarAssign(AstNode* var, AstNode* expr)
{
    AstNode* newNode = MadAlloc(sizeof(AstNode));

    newNode->tag = AST_VARASSIGN;
    newNode->AST_VARASSIGN.var = var;
    newNode->AST_VARASSIGN.expr = expr;

    return newNode;
}

AstNode* newAstArgs(AstNode* arg, AstNode* next)
{
    AstNode* newNode = MadAlloc(sizeof(AstNode));

    newNode->tag = AST_ARGS;
    newNode->AST_ARGS.arg = arg;
    newNode->AST_ARGS.next = next;

    return newNode;
}

AstNode* newAstProcCall(MadaToken procName, AstNode* args)
{
    AstNode* newNode = MadAlloc(sizeof(AstNode));

    newNode->tag = AST_PROC_CALL;
    newNode->AST_PROC_CALL.procName = procName;
    newNode->AST_PROC_CALL.args = args;

    return newNode;
}

AstNode* newAstFor(AstNode* iterator, AstNode* condition, AstNode* statements)
{
    AstNode* newNode = MadAlloc(sizeof(AstNode));

    newNode->tag = AST_FOR;
    newNode->AST_FOR.iterator = iterator;
    newNode->AST_FOR.condition = condition;
    newNode->AST_FOR.statements = statements;

    return newNode;

}


AstNode* newAstCompoundStmt(AstNode* statement, AstNode* next)
{
    AstNode* newNode = MadAlloc(sizeof(AstNode));

    newNode->tag = AST_COMPOUND_STMT;
    newNode->AST_COMPOUND_STMT.statement = statement;
    newNode->AST_COMPOUND_STMT.next = next;

    return newNode;
}

AstNode* newAstStmt(AstNode* statement)
{
    AstNode* newNode = MadAlloc(sizeof(AstNode));
    newNode->tag = AST_STMT;
    newNode->AST_COMPOUND_STMT.statement = statement;

    return newNode;
}

AstNode* newAstBlock(AstNode* compound_stmt)
{
    AstNode* newNode = MadAlloc(sizeof(AstNode));

    newNode->tag = AST_BLOCK;
    newNode->AST_BLOCK.compound_stmt = compound_stmt;

    return newNode;
}

AstNode* newAstProcDecl(MadaToken procName, AstNode* args, AstNode* procBlock, AstTag returnType)
{
    AstNode* newNode = MadAlloc(sizeof(AstNode));

    newNode->tag = AST_PROC_DECL;
    newNode->AST_PROC_DECL.procName = procName;
    newNode->AST_PROC_DECL.args = args;
    newNode->AST_PROC_DECL.procBlock = procBlock;
    newNode->AST_PROC_DECL.returnType = returnType;

    return newNode;
}

AstNode* newAstVarDecl(MadaToken var, MadaToken type)
{
    AstNode* newNode = MadAlloc(sizeof(AstNode));

    newNode->tag = AST_VAR_DECL;
    newNode->AST_VAR_DECL.var = var;
    newNode->AST_VAR_DECL.type = type;

    return newNode;
}

AstNode* newAstAlg(MadaToken id, AstNode* decls, AstNode* algBlock)
{
    AstNode* newNode = MadAlloc(sizeof(AstNode));

    newNode->tag = AST_ALG;
    newNode->AST_ALG.id = id;
    newNode->AST_ALG.decls = decls;
    newNode->AST_ALG.algBlock = algBlock;

    return newNode;
}

AstNode* destroyAst(AstNode* root)
{
    if(root)
    {
        switch(root->tag)
        {
            case AST_VAR_DECL:
            case AST_VAR:
            case AST_VALUE:
                {
                    free(root);
                    break;
                }
            case AST_UNOP:
                {
                    destroyAst(root->AST_UNOP.right);
                    free(root);
                    break;
                }
            case AST_BINOP:
                {
                    destroyAst(root->AST_BINOP.left);
                    destroyAst(root->AST_BINOP.right);
                    free(root);
                    break;
                }
            case AST_VARASSIGN:
                {
                    destroyAst(root->AST_VARASSIGN.expr);
                    free(root);
                    break;
                }
            case AST_ARGS:
                {
                    AstNode* acum = root, *delNode=NULL;
                    while(acum)
                    {
                        destroyAst(acum->AST_ARGS.arg);
                        delNode = acum;
                        acum = acum->AST_ARGS.next;
                        free(delNode);
                    }

                    break;
                }
            case AST_PROC_CALL:
                {
                    destroyAst(root->AST_PROC_CALL.args);
                    free(root);
                    break;
                }
            case AST_BLOCK:
                {
                    destroyAst(root->AST_BLOCK.compound_stmt);
                    free(root);
                    break;
                }
            case AST_COMPOUND_STMT:
                {
                    AstNode* acum = root, *delNode=NULL;
                    while(acum)
                    {
                        destroyAst(acum->AST_COMPOUND_STMT.statement);
                        delNode = acum;
                        acum = acum->AST_COMPOUND_STMT.next;
                        free(delNode);
                    }

                    break;
                }
            case AST_ALG:
                {
                    destroyAst(root->AST_ALG.decls);
                    destroyAst(root->AST_ALG.algBlock);
                }
            default:
                break;
        }
    }
}

void printAst(AstNode* node, int level)
{
    if(node)
    {
        switch(node->tag)
        {
            case AST_BINOP:
                {
                    MadaToken op = node->AST_BINOP.operator;
                    printf("%*cOperador: %.*s\n", level, ' ',op.length, op.in_source_pos);
                    printAst(node->AST_BINOP.left, level+2);
                    printAst(node->AST_BINOP.right, level+2);
                    break;
                }
            case AST_UNOP:
                {
                    MadaToken op = node->AST_UNOP.operator;
                    printf("%*cOperador: %.*s\n", level, ' ',op.length, op.in_source_pos);
                    printAst(node->AST_UNOP.right, level+2);
                    break;
                }
            case AST_VALUE:
                {
                    MadaToken val = node->AST_VALUE.value;
                    printf("%*cValor: %.*s\n", level, ' ', val.length, val.in_source_pos);
                    break;
                }
            case AST_VAR:
                {
                    MadaToken var = node->AST_VAR.var;
                    printf("%*cVariable: %.*s\n", level, ' ', var.length, var.in_source_pos);
                    break;
                }
            case AST_VARASSIGN:
                {
                    printf("%*cAsignacion a:\n", level, ' ');
                    printAst(node->AST_VARASSIGN.var, level+2);
                    printAst(node->AST_VARASSIGN.expr, level+2);
                    break;
                }
            case AST_VAR_DECL:
                {
                    MadaToken var = node->AST_VAR_DECL.var;
                    MadaToken type = node->AST_VAR_DECL.type;
                    printf("%*cDeclaracion: %.*s de tipo %.*s\n", level, ' ', var.length, var.in_source_pos, type.length, type.in_source_pos);
                    break;
                }
            case AST_ARGS:
                {
                    AstNode* acum = node;
                    while(acum)
                    {
                        printAst(acum->AST_ARGS.arg, level+2);
                        acum = acum->AST_ARGS.next;
                    }
                    break;
                }
            case AST_PROC_CALL:
                {
                    MadaToken procName = node->AST_PROC_CALL.procName;
                    printf("%*cLlamada a Proc: %.*s\n", level, ' ', procName.length, procName.in_source_pos);
                    printAst(node->AST_PROC_CALL.args, level+2);
                    break;
                }
            case AST_BLOCK:
                {
                    printf("%*cInicio de bloque.\n", level, ' ');
                    printAst(node->AST_BLOCK.compound_stmt, level);
                    printf("%*cFin de bloque.\n", level, ' ');
                    break;
                }
            case AST_COMPOUND_STMT:
                {
                    AstNode* acum = node;
                    while(acum)
                    {
                        printAst(acum->AST_COMPOUND_STMT.statement, level+2);
                        acum = acum->AST_COMPOUND_STMT.next;
                    }
                    break;
                }
            case AST_STMT:
                {
                    printAst(node->AST_STMT.stmt, level+2);
                    break;
                }
            case AST_ALG:
                {
                    printf("%*cAlgoritmo: ", level, ' ');
                    printTokenStringLit(node->AST_ALG.id);
                    printf("\n");
                    printAst(node->AST_ALG.decls, level+2);
                    printAst(node->AST_ALG.algBlock, level+2);
                    break;
                }
            default:
                break;
        }
    }
}

