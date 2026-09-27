#ifndef TABLE
#define TABLE

#include "status.h"

typedef struct KeySpace {
    int busy;
    unsigned int key;
    char *info;
} KeySpace;

typedef struct Table {
    KeySpace *ks;
    size_t msize;
    size_t csize;
} Table;

Table *createTable(int msize);
char *getItem(Table *table, unsigned int key, Status *err);
void freeTable(Table *table);
Status insertItem(Table *table, unsigned int key, char *info);
Status delItem(Table *table, unsigned int key);

#endif
