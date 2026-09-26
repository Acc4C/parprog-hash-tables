#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "table.h"
#include "myio.h"
#define BLOCK 30

char *getStr(int *err){
    int blockCount = 0;
    char *result = (char*)malloc(sizeof(char)*(BLOCK + 1));
    if (result == NULL){
        *err = 1;
        return NULL;
    }
    result[0] = '\0';
    scanf("\n");
    *err = scanf("%30[^ \t\n]", result + BLOCK*blockCount);
    while (strlen(result) == BLOCK*(blockCount + 1)){
        blockCount += 1;
        result = (char*)realloc(result, sizeof(char)*(BLOCK*(blockCount + 1) + 1));
        if (result == NULL){
            *err = 1;
            return NULL;
        }
        *err = scanf("%30[^ \t\n]", result + BLOCK*blockCount);
    }
    if (*err == -1){
        free(result);
        return NULL;
    }
    result = (char*)realloc(result, sizeof(char)*(strlen(result) + 1));
    if (result == NULL){
        *err = 1;
        return NULL;
    }
    *err = 0;
    return result;
}

int getInt(int *result){
    int err = scanf("%d", result);
    if (err != 1){
        scanf("\n");
        scanf("%*[^\n]");
    }
    if (err == 0){ err = 4; }
    else if (err == 1){ err = 0; }
    return err;
}

int getUnsignedInt(unsigned int *result){
    int err = scanf("%u", result);
    if (err != 1){
        scanf("\n");
        scanf("%*[^\n]");
    }
    if (err == 0){ err = 4; }
    else if (err == 1){ err = 0; }
    return err;
}

int getSize_t(size_t *result){
    int err = scanf("%zu", result);
    if (err != 1){
        scanf("\n");
        scanf("%*[^\n]");
    }
    if (err == 0){ err = 4; }
    else if (err == 1){ err = 0; }
    return err;
}

void printError(int err){
    switch (err){
        case 0:
            break;
        case 1:
            printf("Memory error\n");
            break;
        case 2:
            printf("Элемента с таким ключём и номером версии не существует\n");
            break;
        case 3:
            printf("Элементов с таким ключём не существует\n");
            break;
        case 4:
            printf("Некорректный ввод\n");
            break;
        case 5:
            printf("Файл не найден\n");
            break;
        case 6:
            printf("Нужно ввести простое число\n");
    }
}

void printKeySpaceArray(KeySpace *array){
    if (array -> info == NULL){
        printf("Список пуст\n");
        return;
    }
    printf("key      release    info\n");
    for (KeySpace *item = array; item -> info != NULL; item++){
        if (item -> busy == 1){ printf("%-8u %-10d %s\n", item -> key, item -> release, item -> info); }
    }
}

void printTable(Table *table){
    if (table -> csize == 0){
        printf("В таблице нет элементов\n");
        return;
    }
    printf("busy   key      release    info\n");
    KeySpace *lastItem = (table -> ks + (table -> msize));
    for (KeySpace *item = (table -> ks); item != lastItem; item++){
        if (item -> busy == 1){ printf("1      %-8u %-10d %s\n", item -> key, item -> release, item -> info); }
        else{ printf("%-6d ...      ...        ...\n", item -> busy); }
    }
}
