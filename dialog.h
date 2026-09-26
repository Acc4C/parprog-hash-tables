#ifndef DIALOG
#define DIALOG

int dialogInsertItem(Table *table);
int dialogDelItems(Table *table);
int dialogGetItemsByKey(Table *table);
int dialogImportFromFile(Table **table);
int dialogExportInFile(Table* table);
int dialogGetItemsByKeyAndVersion(Table* table);

#endif
