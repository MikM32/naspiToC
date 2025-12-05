#include "memory.h"



void* MadAlloc(size_t size)
{
    void* ptr = malloc(size);
    if(!ptr)
    {
        fprintf(stderr, "MemoryError: memoria fragmentada o sin espacio.\n");
        exit(0xF);
    }

    return ptr;
}
