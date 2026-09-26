#ifndef TABLE
#define TABLE

#include "status.h"

typedef struct KeySpace {
    int busy;
    unsigned int key;
    int release;
    char *info;
} KeySpace;

typedef struct Table {
    KeySpace *ks;
    size_t msize;
    size_t csize;
} Table;

Table *createTable(int msize);
KeySpace *getItemsByKey(Table *table, unsigned int key, Status *err);
KeySpace *getItemsByKeyAndVersion(Table *table, unsigned int key, int version, Status *err);
void freeTable(Table *table);
Status insertItem(Table *table, unsigned int key, char *info);
void freeKeySpaceArray(KeySpace *array);
Status delItems(Table *table, unsigned int key);

#endif
