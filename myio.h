#ifndef MYIO
#define MYIO
#include <stdio.h>

char *getStr(int *err);
int getInt(int *result);
int getUnsignedInt(unsigned int *result);
int getSize_t(size_t *result);
void printError(int err);
void printKeySpaceArray(KeySpace *array);
void printTable(Table *table);

#endif
