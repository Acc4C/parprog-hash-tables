#include <stdio.h>
#include <stdlib.h>
#include "table.h"
#include "myio.h"
#include "dialog.h"

Status dialogInsertItem(Table *table){
    Status err;
    unsigned int key;
    printf("Введите ключ: ");
    err = getUnsignedInt(&key);
    if (err){ return err; }
    printf("Введите значение элемента: ");
    char *info = getStr(&err);
    if (err){ return err; }
    err = insertItem(table, key, info);
    if (err != OK){
        free(info);
        return err;
    }
    printf("Добавлен элемент с ключём %u и значением \"%s\"\n", key, info);
    free(info);
    return OK;
}

Status dialogDelItems(Table *table){
    Status err;
    unsigned int key;
    printf("Введите ключ: ");
    err = getUnsignedInt(&key);
    if (err != OK){ return err; }
    err = delItems(table, key);
    if (err == OK){ printf("Злементы с ключём %u удалёны\n", key); }
    return err;
}

Status dialogGetItemsByKey(Table *table){
    Status err;
    unsigned int key;
    printf("Введите ключ: ");
    err = getUnsignedInt(&key);
    if (err){ return err; }
    KeySpace *array = getItemsByKey(table, key, &err);
    if (err == OK){
        printKeySpaceArray(array);
        freeKeySpaceArray(array);
    }
    return err;
}

Status dialogGetItemsByKeyAndVersion(Table *table){
    int err, version;
    char flag;
    unsigned int key;
    printf("Введите ключ и его версию: ");
    err = getUnsignedInt(&key);
    if (err != OK){ return err; }
    scanf("\n");
    err = scanf("%[*]", &flag);
    if (err == -1){ return END_OF_FILE; }
    if (err == 1){ version = 0; }
    else{
        err = getInt(&version);
        if (err != OK){ return err; }
        if (version < 1){ return ERR_INCORRECT_INPUT; }
    }
    KeySpace *array = getItemsByKeyAndVersion(table, key, version, &err);
    if (err == OK){
        printKeySpaceArray(array);
        freeKeySpaceArray(array);
    }
    return err;
}

Status dialogImportFromFile(Table **table){
    Status err;
    char *path;
    printf("Введите путь к файлу: ");
    path = getStr(&err);
    if (err != OK){ return err; }
    Table *newTable = importFromFile(path, &err);
    free(path);
    if (err == OK){
        freeTable(*table);
        *table = newTable;
        printf("Импортирована таблица из %ld ячейки(-ек)\n", (*table) -> msize);
    }
    return err;
}

Status dialogExportInFile(Table *table){
    Status err;
    char *path;
    printf("Введите путь к файлу: ");
    path = getStr(&err);
    if (err != OK){ return err; }
    err = exportInFile(table, path);
    if (err == OK){
        printf("Таблица экспортирована в файл \"%s\"\n", path);
    }
    free(path);
    return err;
}
