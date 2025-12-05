#include "sem.h"

char callableTable[][20] =
{
    [CALL_WRITE]    = {"printf"},
    [CALL_READ]     = {"scanf"},
};

Symbol* newSymVar(int scope, AstTag type, MadaToken token, MadaToken procName)
{
    Symbol* var = MadAlloc(sizeof(Symbol));
    var->type = SYM_VAR;
    var->variable.type = type;
    var->variable.token = token;
    var->variable.procName = procName;
    var->scope = scope;

    return var;
}

Symbol* newSymCall(int scope, MadaToken name, int argnum, AstTag returnType, bool needTypeCheck, CallType ct)
{
    Symbol* callable = MadAlloc(sizeof(Symbol));

    callable->type = SYM_CALL;

    callable->scope = scope;
    callable->callable.arg_num = argnum;
    callable->callable.name = name;
    callable->callable.needTypeCheck = needTypeCheck;
    callable->callable.returnType = returnType;
    callable->callable.type = ct;

    return callable;
}

void initMadaSemantic(MadaSemantic* sem)
{
    sem->currentScope = GLOBAL_SCOPE;
    sem->exprStackIndex = 0;
    initMadaParser(&sem->parser);
    initHt(&sem->symbolTable);
    initHt(&sem->typeTable);

    #ifdef OUTPUT_SCREEN
        sem->outputBuffer = stdout;
    #else
        sem->outputBuffer = fopen("converted.c", "w");
    #endif // OUTPUT_SCREEN

    //MadaToken tok;
    AstTag* typeval = malloc(sizeof(AstTag));
    *typeval = AST_INTEGER;
    htSet(&sem->typeTable, newEntryKey("entero", 6), typeval);

    typeval = malloc(sizeof(AstTag));
    *typeval = AST_BOOL;
    htSet(&sem->typeTable, newEntryKey("bool", 4), typeval);

    typeval = malloc(sizeof(AstTag));
    *typeval = AST_REAL;
    htSet(&sem->typeTable, newEntryKey("real", 4), typeval);

    MadaToken voidToken;
    initMadaToken(&voidToken, TOKEN_NONE, 0, NULL, 0, 0);

    Symbol* sym = newSymCall(GLOBAL_SCOPE, voidToken, MAX_ARGS, AST_NONE,false, CALL_WRITE);
    htSet(&sem->symbolTable, newEntryKey("escribir", 8), sym);
}

void pushExprStack(MadaSemantic* sem, AstTag typeval)
{
    if(sem->exprStackIndex+1 <= MAX_EXPR)
    {
        sem->exprStack[sem->exprStackIndex++] = typeval;
    }

}

AstTag popExprStack(MadaSemantic* sem)
{
    if(sem->exprStackIndex-1 > -1)
    {
        return sem->exprStack[--sem->exprStackIndex];
    }
    else
    {
        return AST_NONE;
    }
}

AstTag resolveType(MadaSemantic* sem, MadaToken typetok)
{
    uint32_t hashType = hash(typetok.in_source_pos, typetok.length);

    EntryKey* typeKey = htFindKey(&sem->typeTable, typetok.in_source_pos, typetok.length, hashType);

    if(!typeKey)
    {
        return AST_NONE;
    }
    else
    {
        AstTag* type = NULL;
        htGet(&sem->typeTable, typeKey, &type);

        return *type;
    }
}

void getCOperator(MadaSemantic* sem, MadaToken token)
{
    if(token.type == TOKEN_OR)
    {
        fprintf(sem->outputBuffer, "||");
    }
    else if(token.type == TOKEN_AND)
    {
        fprintf(sem->outputBuffer, "&&");
    }
    else
    {
        printTokenStringEx(token, sem->outputBuffer);
    }
}

void walkBlock(MadaSemantic* sem, AstNode* node, bool output)
{
    fprintf(sem->outputBuffer,"%*c{\n", sem->currentScope*4, ' ');
    sem->currentScope++;
    walk(sem, node->AST_BLOCK.compound_stmt, output);
    sem->currentScope--;
    fprintf(sem->outputBuffer,"%*c}\n", sem->currentScope*4, ' ');

}

void walkBinOp(MadaSemantic* sem, AstNode* node, bool output)
{
    walk(sem, node->AST_BINOP.left, output);

    if(output) getCOperator(sem, node->AST_BINOP.operator);
    walk(sem, node->AST_BINOP.right, output);

    AstTag t2=popExprStack(sem), t1=popExprStack(sem);

    TokenType optype = node->AST_BINOP.operator.type;

    if(t1 == AST_BOOL || t2 == AST_BOOL)
    {
        if(optype != TOKEN_AND && optype != TOKEN_OR && optype != TOKEN_NOT && optype != TOKEN_EQU)
        {
            printf("/*el operador maneja otro tipo de datos que no es booleano*/");
        }
    }
    if(t1 != t2)
    {
        printf("/*Error tipos incompatibles*/");
    }
    pushExprStack(sem, t1);


}

void walkVar(MadaSemantic* sem, AstNode* node, bool output)
{
    MadaToken token_var = node->AST_VAR.var;
    uint32_t hash_var = hash(token_var.in_source_pos, token_var.length);

    EntryKey* var = htFindKey(&sem->symbolTable, token_var.in_source_pos, token_var.length, hash_var);

    if(!var)
    {
        errorAt(&sem->parser, token_var, "La variable no ha sido declarada");
    }
    else
    {
        Symbol* symvar=NULL;
        htGet(&sem->symbolTable, var, &symvar);

        if(symvar->type != SYM_VAR)
        {
            errorAt(&sem->parser, token_var, "La variable no ha sido declarada");
        }
        else if(symvar->scope != sem->currentScope && !tokenEqual(symvar->variable.procName, sem->currentProcName))
        {
            errorAt(&sem->parser, token_var, "La variable no ha sido declarada en este contexto");
        }
        else
        {
            pushExprStack(sem, symvar->variable.type);

            if(output) printTokenStringEx(token_var, sem->outputBuffer);
        }

    }


}

void walkValue(MadaSemantic* sem, AstNode* node, bool output)
{
    //printf("%*c", sem->currentScope*4, ' ');
    if(output) {

            if(node->AST_VALUE.vtype == AST_STR_LIT) // si es un literal de cadena de caracteres
            {
                //imprime el texto del token encerrado entre comillas
                printTokenStringLitEx(node->AST_VALUE.value, sem->outputBuffer);
            }
            else printTokenStringEx(node->AST_VALUE.value, sem->outputBuffer);
    }

    if(node->AST_VALUE.vtype == AST_TRUE || node->AST_VALUE.vtype == AST_FALSE)
    {
        pushExprStack(sem, AST_BOOL);
    }
    else
    {
        pushExprStack(sem, node->AST_VALUE.vtype);
    }

}

void walkStmt(MadaSemantic* sem, AstNode* node, bool output)
{
    walk(sem, node->AST_STMT.stmt, output);

    if(output) fprintf(sem->outputBuffer,";\n");
}

void walkCompoundStmt(MadaSemantic* sem, AstNode* node, bool output)
{
    AstNode* acum=node;

    while(acum)
    {
        fprintf(sem->outputBuffer,"%*c", sem->currentScope*4,' ');
        walk(sem, acum->AST_COMPOUND_STMT.statement, output);
        acum = acum->AST_COMPOUND_STMT.next;

    }

}


void walkForStmt(MadaSemantic* sem, AstNode* node, bool output)
{
    MadaToken vartok;

    AstNode* it = node->AST_FOR.iterator;

    if(it->tag == AST_VAR)
    {
        vartok = it->AST_VAR.var;
    }
    else
    {
        vartok = it->AST_VARASSIGN.var->AST_VAR.var;
    }

    fprintf(sem->outputBuffer, "for(");
    walk(sem, it);

    AstTag ittype = popExprStack(sem);
    if(ittype != AST_INTEGER && ittype != AST_REAL)
    {
        errorAtPrevious(&sem->parser, "el tipo de la variable no puede ser un iterador");
    }

    fprintf(sem->outputBuffer, "; ");
    printTokenStringEx(vartok, sem->outputBuffer);
    fprintf(sem->outputBuffer, "<=");
    walk(sem, node->AST_FOR.condition);

    AstTag condtype = popExprStack(sem);
    if(condtype != AST_INTEGER && condtype != AST_REAL)
    {
        errorAtPrevious(&sem->parser, "el limite del bucle no es iterable");
    }

    fprintf(sem->outputBuffer, "; ");
    printTokenStringEx(vartok, sem->outputBuffer);
    fprintf(sem->outputBuffer, "++)\n");
    walk(sem, node->AST_FOR.statements);

}


//Funcion que retorna la cadena del tipo correspondiente en lenguaje C
void getCType(MadaSemantic* sem, AstTag type)
{
    switch(type)
    {
    case AST_INTEGER:
        {
            fprintf(sem->outputBuffer, "int");
            break;
        }
    case AST_REAL:
        {
            fprintf(sem->outputBuffer, "float");
            break;
        }
    case AST_BOOL:
        {
            fprintf(sem->outputBuffer, "bool");
            break;
        }
    }
}

void resolveNativeProcs(MadaSemantic* sem, MadaToken token, CallType type)
{
    if(type != CALL_USERDEF)
    {
        fprintf(sem->outputBuffer, "%s", callableTable[type]);
    }
    else
    {
        printTokenStringEx(token, sem->outputBuffer);
    }
}

void walkVarDecl(MadaSemantic* sem, AstNode* node, bool output)
{
    MadaToken var = node->AST_VAR_DECL.var;
    MadaToken type = node->AST_VAR_DECL.type;
    uint32_t hashVar = hash(var.in_source_pos, var.length);

    if(!htFindKey(&sem->symbolTable, var.in_source_pos, var.length, hashVar))
    {
        //printTokenString(type);
        AstTag resolvedType = resolveType(sem, type);
        getCType(sem, resolvedType);
        fprintf(sem->outputBuffer," ");
        printTokenStringEx(var, sem->outputBuffer);


        htSet(&sem->symbolTable, newEntryKey(var.in_source_pos, var.length), newSymVar(sem->currentScope, resolvedType, var, sem->currentProcName));
    }
    else
    {
        errorAt(&sem->parser, var, "La variable ya ha sido declarada con anterioridad");
    }
}

void walkVarAssign(MadaSemantic* sem, AstNode* node, bool output)
{
    walkVar(sem, node->AST_VARASSIGN.var, output);
    AstTag vartype = popExprStack(sem);
    fprintf(sem->outputBuffer," = ");
    walk(sem, node->AST_VARASSIGN.expr, output);
    AstTag exprtype = popExprStack(sem);

    if(vartype != exprtype)
    {
        printf("/*el tipo de la variable no corresponde al tipo de la expresion*/");
    }

    pushExprStack(sem, vartype);

}

void getStringFormat(MadaSemantic* sem, AstNode* node)
{
     fprintf(sem->outputBuffer,"\"");
     AstNode* current_arg = node;
     while(current_arg)
    {

        walk(sem, current_arg->AST_ARGS.arg, false);

        AstTag argtype = popExprStack(sem);

        switch(argtype)
        {
            case AST_BOOL:
            case AST_INTEGER: { fprintf(sem->outputBuffer,"%cd", '%'); break;}
            case AST_REAL: { fprintf(sem->outputBuffer,"%cf", '%'); break;}
            case AST_STR_LIT: {fprintf(sem->outputBuffer,"%cs", '%'); break;}
        }

        current_arg = current_arg->AST_ARGS.next;
        if(current_arg) fprintf(sem->outputBuffer," ");
    }

    fprintf(sem->outputBuffer,"\"");
}

void walkProcCall(MadaSemantic* sem, AstNode* node, bool output)
{

    MadaToken procName = node->AST_PROC_CALL.procName;
    EntryKey* procKey = htFindKey(&sem->symbolTable,
                                 procName.in_source_pos, procName.length,
                                 hash(procName.in_source_pos, procName.length));


    if(!procKey) //No esta en la tabla de simbolos
    {
        errorAt(&sem->parser, procName, "el procedimiento no ha sido declarado");
    }
    else
    {
        Symbol* symproc = NULL;
        htGet(&sem->symbolTable, procKey, &symproc);


        if(symproc->type == SYM_CALL)
        {
            if(symproc->callable.arg_num > MAX_ARGS) errorAt(&sem->parser, procName, "se excedio el maximo de argumentos");

            AstNode* current_arg = node->AST_PROC_CALL.args;

            //printTokenString(procName);
            resolveNativeProcs(sem, procName, symproc->callable.type);
            fprintf(sem->outputBuffer,"(");



            if(symproc->callable.type == CALL_WRITE || symproc->callable.type == CALL_READ)
            {
                walk(sem, current_arg, false);
                getStringFormat(sem, current_arg);
                fprintf(sem->outputBuffer,", ");
            }

            int current_argnum = 0;

            while(current_arg)
            {

                walk(sem, current_arg->AST_ARGS.arg, true);

                if(symproc->callable.needTypeCheck)
                {

                }

                current_arg = current_arg->AST_ARGS.next;
                if(current_arg) fprintf(sem->outputBuffer,", ");
                current_argnum++;
            }

            if(symproc->callable.needTypeCheck && symproc->callable.arg_num != current_argnum)
            {
                errorAt(&sem->parser, procName, "Faltan o sobran argumentos en el procedimiento");
            }

            fprintf(sem->outputBuffer,")");

        }else //Esta en la tabla de simbolos pero no es una funcion o procedimiento
        {
            errorAt(&sem->parser, procName, "el procedimiento no ha sido declarado");
        }

    }
}

void walkProcDecl(MadaSemantic* sem, AstNode* node, bool output)
{

    MadaToken name = node->AST_PROC_DECL.procName;
    uint32_t hashName = hash(name.in_source_pos, name.length);
    sem->argDecl = true;

    if(!htFindKey(&sem->symbolTable, name.in_source_pos, name.length, hashName))
    {
        int argNum = 0;

        if(output)
        {
            fprintf(sem->outputBuffer,"void ");
            printTokenStringEx(name, sem->outputBuffer);
            fprintf(sem->outputBuffer,"(");
            AstNode* current_arg = node->AST_PROC_DECL.args;

            while(current_arg)
            {

                walk(sem, current_arg->AST_ARGS.arg, output);


                current_arg = current_arg->AST_ARGS.next;
                argNum++;
                if(current_arg) fprintf(sem->outputBuffer,", ");
            }

            fprintf(sem->outputBuffer,")\n");
        }

        walk(sem, node->AST_PROC_DECL.procBlock, output);

        Symbol* symProc = newSymCall(sem->currentScope+1, name, argNum, AST_NONE, true, CALL_USERDEF);
        htSet(&sem->symbolTable, newEntryKey(name.in_source_pos, name.length), symProc);
    }
    else
    {
        errorAt(&sem->parser, name, "el procedimiento ya ha sido declarado anteriormente");
    }

    sem->argDecl = false;
}

void walkAlgorithm(MadaSemantic* sem, AstNode* node, bool output)
{
    sem->currentProcName = node->AST_ALG.id;
    if(output)
    {
        fprintf(sem->outputBuffer,"//");
        printTokenStringEx(node->AST_ALG.id, sem->outputBuffer);
        fprintf(sem->outputBuffer,"\n");
        fprintf(sem->outputBuffer,"#include <stdio.h>\n");
    }


    walk(sem, node->AST_ALG.decls, output);

    if(output) fprintf(sem->outputBuffer,"int main()\n");
    walk(sem, node->AST_ALG.algBlock, output);
}

void walk(MadaSemantic* sem, AstNode* node, bool output)
{
    if(node)
    {
        switch(node->tag)
        {
            case AST_VALUE:
                {
                    walkValue(sem, node, output);
                    break;
                }
            case AST_VAR:
                {
                    walkVar(sem, node, output);
                    break;
                }
            case AST_BINOP:
                {
                    walkBinOp(sem, node, output);
                    break;
                }
            case AST_VARASSIGN:
                {
                    walkVarAssign(sem, node, output);
                    break;
                }
            case AST_VAR_DECL:
                {
                    walkVarDecl(sem, node, output);
                    break;
                }
            case AST_PROC_CALL:
                {
                    walkProcCall(sem, node, output);
                    break;
                }
            case AST_PROC_DECL:
                {
                    walkProcDecl(sem, node, output);
                    break;
                }
            case AST_FOR:
                {
                    walkForStmt(sem, node, output);
                    break;
                }
            case AST_BLOCK:
                {
                    walkBlock(sem, node, output);
                    break;
                }
            case AST_STMT:
                {
                    walkStmt(sem, node, output);
                    break;
                }
            case AST_COMPOUND_STMT:
                {
                    walkCompoundStmt(sem, node, output);
                    break;
                }
            case AST_ALG:
                {
                    walkAlgorithm(sem, node, output);
                    break;
                }
        }
    }
}

bool madaSemHadError(MadaSemantic* sem)
{
    return (sem->parser.hadError);
}
