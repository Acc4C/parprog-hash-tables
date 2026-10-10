#include <stdio.h>
#include <stdlib.h>
#include "table.h"
#include "myio.h"
#include "dialog.h"

Status dialogInsertItem(Table *table){
    Status err;
    int key;
    printf("Введите ключ: ");
    err = getInt(&key);
    if (err){
        return err;
    }
    printf("Введите значение элемента: ");
    int info;
    err = getInt(&info);
    if (err){
        return err;
    }
    err = insertItem(table, key, info);
    if (err != OK){
        return err;
    }
    printf("Добавлен элемент с ключём %d и значением %d\n", key, info);
    return OK;
}

Status dialogDelItem(Table *table){
    Status err;
    int key;
    printf("Введите ключ: ");
    err = getInt(&key);
    if (err != OK){
        return err;
    }
    err = delItem(table, key);
    if (err == OK){
        printf("Злемент с ключём %d удалён\n", key);
    }
    return err;
}

Status dialogGetItem(Table *table){
    Status err;
    int key;
    printf("Введите ключ: ");
    err = getInt(&key);
    if (err){
        return err;
    }
    int val = getItem(table, key, &err);
    if (err == OK){
        printf("Элемент с ключём %d: %d\n", key, val);
    }
    return err;
}
