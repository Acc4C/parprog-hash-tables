#ifndef DIALOG
#define DIALOG

#include "status.h"

Status dialogInsertItem(Table *table);
Status dialogDelItems(Table *table);
Status dialogGetItemsByKey(Table *table);
Status dialogGetItemsByKeyAndVersion(Table* table);

#endif
