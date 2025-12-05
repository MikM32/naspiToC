#include <stdio.h>
#include <stdlib.h>

#include "sem.h"


int main()
{
    char* codigo = readFile("test.mc");

    if(codigo)
    {
        MadaSemantic sem;

        initMadaSemantic(&sem);

        loadMadaSource(&sem.parser.lexer, codigo);

        parseNext(&sem.parser);

        AstNode* a = algorithm(&sem.parser);

        printAst(a, 1);

        if(!madaSemHadError(&sem))
        {
            walk(&sem, a, true);
        }

        destroyAst(a);

        //destroyMadaSemantic

        free(codigo);
    }


    return 0;
}
