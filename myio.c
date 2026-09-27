#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "table.h"
#include "myio.h"
#define BLOCK 30

char *getStr(Status *err){
    int blockCount = 0;
    char *result = (char*)malloc(sizeof(char)*(BLOCK + 1));
    if (result == NULL){
        *err = ERR_MEMORY_ERROR;
        return NULL;
    }
    result[0] = '\0';
    scanf("\n");
    *err = scanf("%30[^ \t\n]", result + BLOCK*blockCount);
    while (strlen(result) == BLOCK*(blockCount + 1)){
        blockCount += 1;
        result = (char*)realloc(result, sizeof(char)*(BLOCK*(blockCount + 1) + 1));
        if (result == NULL){
            *err = ERR_MEMORY_ERROR;
            return NULL;
        }
        *err = scanf("%30[^ \t\n]", result + BLOCK*blockCount);
    }
    if (*err == END_OF_FILE){
        free(result);
        return NULL;
    }
    result = (char*)realloc(result, sizeof(char)*(strlen(result) + 1));
    if (result == NULL){
        *err = ERR_MEMORY_ERROR;
        return NULL;
    }
    *err = OK;
    return result;
}

Status getInt(int *result){
    int err = scanf("%d", result);
    if (err != 1){
        scanf("\n");
        scanf("%*[^\n]");
    }
    if (err == 0){
        return ERR_INCORRECT_INPUT;
    } else if (err == 1){
        return OK;
    } else{
        return END_OF_FILE;
    }
}

Status getSize_t(size_t *result){
    int err = scanf("%zu", result);
    if (err != 1){
        scanf("\n");
        scanf("%*[^\n]");
    }
    if (err == 0){
        return ERR_INCORRECT_INPUT;
    } else if (err == 1){
        return OK;
    } else{
        return END_OF_FILE;
    }
}

void printError(Status err){
    switch (err){
        case OK:
            break;
        case ERR_MEMORY_ERROR:
            printf("Memory error\n");
            break;
        case ERR_NOT_FOUND:
            printf("Элемента с таким ключём не существует\n");
            break;
        case ERR_INCORRECT_INPUT:
            printf("Некорректный ввод\n");
            break;
        case ERR_NOT_PRIME_NUMBER:
            printf("Нужно ввести простое число\n");
    }
}

void printTable(Table *table){
    if (table -> csize == 0){
        printf("В таблице нет элементов\n");
        return;
    }
    printf("busy   key          info\n");
    KeySpace *lastItem = (table -> ks + (table -> msize));
    for (KeySpace *item = (table -> ks); item != lastItem; item++){
        if (item -> busy == 1){ printf("1      %-12s %s\n", item -> key, item -> info); }
        else{ printf("%-6d ...      ...\n", item -> busy); }
    }
}
