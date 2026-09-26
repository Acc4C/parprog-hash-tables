#ifndef MYIO
#define MYIO
#include <stdio.h>
#include "status.h"

char *getStr(Status *err);
Status getInt(int *result);
Status getUnsignedInt(unsigned int *result);
Status getSize_t(size_t *result);
void printError(Status err);
void printKeySpaceArray(KeySpace *array);
void printTable(Table *table);

#endif
