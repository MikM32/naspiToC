#include "utils.h"

void madaMessage(char* msg)
{
    int limit = strlen(msg);
    int spaceLimit = ((limit/6)-1);

    limit+= spaceLimit*2;

    for(int i=0; i<limit+1; i++)
    {
        printf("=");
    }
    printf("\n");
    printf("|");
    for(int i=0; i<spaceLimit; i++)
    {
        printf(" ");
    }
    printf(msg);
    for(int i=0; i<spaceLimit-1; i++)
    {
        printf(" ");
    }
    printf("|");
    printf("\n");
    for(int i=0; i<limit+1; i++)
    {
        printf("=");
    }
    printf("\n");


}
/*
void initVector(Vector* vec)
{
    vec->capacity = 16;
    vec->count = 0;
    vec->data = MadAlloc(sizeof(void*)*vec->capacity);
}

Vector* newVector()
{
    Vector* newVec = MadAlloc(sizeof(Vector));
    initVector(newVec);

    return newVec;
}

void growVector(Vector* vec)
{
    vec->capacity *= 2;
    vec->data = realloc(vec->data, vec->capacity);
    if(!vec->data)
    {
        fprintf(stderr, "MemoryError: memoria fragmentada o sin espacio.\n");
        exit(0xF);
    }
}

void* vectorLast(Vector* vec)
{
    if(vec->count) return &vec->data[vec->count-1];
    return NULL;
}

void vectorPush(Vector* vec, void* elem)
{
    if(vec->count == vec->capacity)
    {
        growVector(vec);
    }
    vec->data[vec->count++] = elem;
}*/

char* readFile(char* path)
{
    FILE* f = fopen(path, "r");

    if(!f)
    {
        fprintf(stderr, "MADA: Error al abrir el archivo: No se encuentra \'%s\'", path);
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    int length = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* buffer = MadAlloc(sizeof(char)*length);

    fread(buffer, sizeof(char), length, f);

    fclose(f);

    return buffer;

}

uint32_t hash(char* str, int length)
{
    uint32_t hash=FNV32_OFFSET;

    for(int i=0; i<length; i++)
    {
        hash ^= (uint32_t)str[i]; // hash = hash XOR str[i]
        hash *= FNV32_PRIME;
    }

    return hash;
}


EntryKey* newEntryKey(char* str, int length)
{
    EntryKey* key = MadAlloc(sizeof(EntryKey));
    key->str = str;
    key->length = length;
    key->hash = hash(str, key->length);

    return key;
}

void initHt(HashTable* ht)
{
    ht->capacity = 0;
    ht->count = 0;
    ht->entries = NULL;
    adjustEntries(ht, 16);
}





Entry* findEntry(Entry* entries, int capacity, EntryKey* key)
{
    uint32_t index = key->hash % capacity;
    while(true)
    {
        Entry* entry = &entries[index];

        if(entry->key == key || entry->key == NULL)
        {
            return entry;
        }

        index = (index+1) % capacity;
    }
}

void adjustEntries(HashTable* ht, int newCapacity)
{
    Entry* newEntries = MadAlloc(sizeof(Entry)*newCapacity);

    for(int i=0; i<newCapacity; i++)
    {
        newEntries[i].key = NULL;
        newEntries[i].value = NULL;
    }

    for(int i=0; i<ht->capacity; i++)
    {
        Entry* entry = &ht->entries[i];
        if (entry->key == NULL) continue;

        Entry* dest = findEntry(newEntries, newCapacity, entry->key);
        dest->key = entry->key;
        dest->value = entry->value;
    }

    if(ht->entries) free(ht->entries);

    ht->entries = newEntries;
    ht->capacity = newCapacity;

}

bool htSet(HashTable* ht, EntryKey* key, void* value)
{
    if (ht->count + 1 > ht->capacity * TABLE_MAX_LOAD) {
        adjustEntries(ht, ht->capacity*2);
    }

    Entry* entry = findEntry(ht->entries, ht->capacity, key);

    bool isNewKey = entry->key == NULL;
    if(isNewKey) ht->count++;

    entry->key = key;
    entry->value = value;
    return isNewKey;
}

bool htGet(HashTable* ht, EntryKey* key, void** value)
{
    if(ht->count == 0) return false;

    Entry* entry = findEntry(ht->entries, ht->capacity, key);
    if(entry->key == NULL) return false;


    *value = entry->value;
    return true;
}

EntryKey* htFindKey(HashTable* ht, const char* str,
                           int length, uint32_t hash) {
  if (ht->count == 0) return NULL;

  uint32_t index = hash % ht->capacity;
  for (;;) {
    Entry* entry = &ht->entries[index];
    if (entry->key == NULL) {
      // Stop if we find an empty non-tombstone entry.
      if (!entry->value) return NULL;
    } else if (entry->key->length == length &&
        entry->key->hash == hash &&
        memcmp(entry->key->str, str, length) == 0) {
      // We found it.
      return entry->key;
    }

    index = (index + 1) % ht->capacity;
  }
}
