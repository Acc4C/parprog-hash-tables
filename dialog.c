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

Status dialogDelItem(Table *table){
    Status err;
    unsigned int key;
    printf("Введите ключ: ");
    err = getUnsignedInt(&key);
    if (err != OK){ return err; }
    err = delItem(table, key);
    if (err == OK){ printf("Злемент с ключём %u удалён\n", key); }
    return err;
}

Status dialogGetItem(Table *table){
    Status err;
    unsigned int key;
    printf("Введите ключ: ");
    err = getUnsignedInt(&key);
    if (err){ return err; }
    char *val = getItem(table, key, &err);
    if (err == OK){
        printf("Элемент с ключём %u: \"%s\"\n", key, val);
    }
    return err;
}
