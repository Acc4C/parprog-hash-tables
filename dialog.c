#include <stdio.h>
#include <stdlib.h>
#include "table.h"
#include "myio.h"
#include "dialog.h"

int dialogInsertItem(Table *table){
    int err;
    unsigned int key;
    printf("Введите ключ: ");
    err = getUnsignedInt(&key);
    if (err){ return err; }
    printf("Введите значение элемента: ");
    char *info = getStr(&err);
    if (err){ return err; }
    err = insertItem(table, key, info);
    if (err){
        free(info);
        return err;
    }
    printf("Добавлен элемент с ключём %u и значением \"%s\"\n", key, info);
    free(info);
    return 0;
}

int dialogDelItems(Table *table){
    int err, version;
    unsigned int key;
    printf("Введите ключ: ");
    err = getUnsignedInt(&key);
    if (err){ return err; }
    err = delItems(table, key);
    if (!err){ printf("Злементы с ключём %u удалёны\n", key); }
    return err;
}

int dialogGetItemsByKey(Table *table){
    int err;
    char *info;
    unsigned int key;
    printf("Введите ключ: ");
    err = getUnsignedInt(&key);
    if (err){ return err; }
    KeySpace *array = getItemsByKey(table, key, &err);
    if (!err){
        printKeySpaceArray(array);
        freeKeySpaceArray(array);
    }
    return err;
}

int dialogGetItemsByKeyAndVersion(Table *table){
    int err, version;
    char flag;
    unsigned int key;
    printf("Введите ключ и его версию: ");
    err = getUnsignedInt(&key);
    if (err){ return err; }
    scanf("\n");
    err = scanf("%[*]", &flag);
    if (err == -1){ return -1; }
    if (err == 1){ version = 0; }
    else{
        err = getInt(&version);
        if (err){ return err; }
        if (version < 1){ return 4; }
    }
    KeySpace *array = getItemsByKeyAndVersion(table, key, version, &err);
    if (!err){
        printKeySpaceArray(array);
        freeKeySpaceArray(array);
    }
    return err;
}

int dialogImportFromFile(Table **table){
    int err;
    char *path;
    printf("Введите путь к файлу: ");
    path = getStr(&err);
    if (err){ return err; }
    Table *newTable = importFromFile(path, &err);
    free(path);
    if (!err){
        freeTable(*table);
        *table = newTable;
        printf("Импортирована таблица из %ld ячейки(-ек)\n", (*table) -> msize);
    }
    return err;
}

int dialogExportInFile(Table *table){
    int err;
    char *path;
    printf("Введите путь к файлу: ");
    path = getStr(&err);
    if (err){ return err; }
    err = exportInFile(table, path);
    if (!err){
        printf("Таблица экспортирована в файл \"%s\"\n", path);
    }
    free(path);
    return err;
}
