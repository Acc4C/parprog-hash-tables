#ifndef TABLE
#define TABLE

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
KeySpace *getItemsByKey(Table *table, unsigned int key, int *err);
KeySpace *getItemsByKeyAndVersion(Table *table, unsigned int key, int version, int *err);
void freeTable(Table *table);
Table *importFromFile(char *path, int *err);
int exportInFile(Table *table, char *path);
int insertItem(Table *table, unsigned int key, char *info);
void freeKeySpaceArray(KeySpace *array);
int delItems(Table *table, unsigned int key);

#endif
