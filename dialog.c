#include <stdio.h>
#include <stdlib.h>
#include "table.h"
#include "myio.h"
#include "dialog.h"

Status dialogInsertItem(Table *table){
    Status err;
    char *key;
    printf("Введите ключ: ");
    key = getStr(&err);
    if (err){ return err; }
    printf("Введите значение элемента: ");
    char *info = getStr(&err);
    if (err){
        free(key);
        return err;
    }
    err = insertItem(table, key, info);
    if (err != OK){
        free(key);
        free(info);
        return err;
    }
    printf("Добавлен элемент с ключём \"%s\" и значением \"%s\"\n", key, info);
    free(key);
    free(info);
    return OK;
}

Status dialogDelItem(Table *table){
    Status err;
    char *key;
    printf("Введите ключ: ");
    key = getStr(&err);
    if (err != OK){ return err; }
    err = delItem(table, key);
    if (err == OK){ printf("Злемент с ключём \"%s\" удалён\n", key); }
    free(key);
    return err;
}

Status dialogGetItem(Table *table){
    Status err;
    char *key;
    printf("Введите ключ: ");
    key = getStr(&err);
    if (err){ return err; }
    char *val = getItem(table, key, &err);
    if (err == OK){
        printf("Элемент с ключём \"%s\": \"%s\"\n", key, val);
        free(val);
    }
    free(key);
    return err;
}
