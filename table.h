#ifndef TABLE
#define TABLE

#include "status.h"

typedef struct KeySpace {
    int busy;
    int key;
    int info;
} KeySpace;

typedef struct Table {
    KeySpace *ks;
    size_t msize;
    size_t csize;
} Table;

Table *createTable(size_t msize);
int getItem(Table *table, int key, Status *err);
size_t getNewMsize(size_t msize);
void clearTable(Table *table);
void freeTable(Table *table);
Status insertItem(Table *table, int key, int info);
Status delItem(Table *table, int key);

#endif
