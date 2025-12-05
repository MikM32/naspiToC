#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "memory.h"

#define FNV32_PRIME 16777619
#define FNV32_OFFSET 2166136261

#define TABLE_MAX_LOAD 0.75

typedef struct Vector
{
    int capacity;
    int count;
    void* data;
}Vector;


typedef struct EntryKey
{
    int length;
    char* str;
    uint32_t hash;
}EntryKey;

typedef struct Entry
{
    EntryKey* key;
    void* value;
}Entry;

typedef struct HashTable
{
    int capacity;
    int count;
    Entry* entries;
}HashTable;

void initVector(Vector* vec);
EntryKey* htFindKey(HashTable* ht, const char* str,
                           int length, uint32_t hash);
void adjustEntries(HashTable* ht, int newCapacity);
Entry* findEntry(Entry* entries, int capacity, EntryKey* key);
Vector* newVector();

char* readFile(char* path);

void madaMessage(char* msg);

#endif // UTILS_H_INCLUDED
