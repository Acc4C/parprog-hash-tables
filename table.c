#include <stdlib.h>
#include <string.h>
#include "table.h"
#include "myio.h"

int hash1(unsigned int key, size_t msize){ return key%msize; }

int hash2(unsigned int key, size_t msize){ return 1 + (key*key)%(msize - 1); }

int hash(unsigned int key, int i, size_t msize){ return (hash1(key, msize) + i*hash2(key, msize))%msize; }

Table *createTable(int msize){
    Table *newTable = (Table*)malloc(sizeof(Table));
    if (newTable != NULL){
        newTable -> ks = (KeySpace*)calloc(msize, sizeof(KeySpace));
        if (newTable -> ks == NULL){
            free(newTable);
            return NULL;
        }
        newTable -> msize = msize;
        newTable -> csize = 0;
    }
    return newTable;
}

char *getItem(Table* table, unsigned int key, Status *err){
    int viewed = 0;
    int index = hash(key, viewed, table -> msize);
    KeySpace *firstItem = (table -> ks);
    while (((firstItem + index) -> busy != 0) && (viewed < (table -> msize))){
        if (((firstItem + index) -> busy == 1) && ((firstItem + index) -> key == key)){
            char *info = (firstItem + index) -> info;
            char *res = (char*)malloc(sizeof(char)*(strlen(info) + 1));
            if (res == NULL){
                *err = ERR_MEMORY_ERROR;
                return NULL;
            }
            strcpy(res, info);
            *err = OK;
            return res;
        }
        viewed++;
        index = hash(key, viewed, table -> msize);
    }
    *err = ERR_NOT_FOUND;
    return NULL;
}

size_t getNewMsize(size_t msize){
    int flag;
    for (size_t i = msize + 1;; i++){
        flag = 1;
        for (size_t j = 2; j*j <= i; j++){
            if (i%j == 0){
                flag = 0;
                break;
            }
        }
        if (flag){ return i; }
    }
}

void putItem(KeySpace* ks, KeySpace newItem, size_t msize){
    int viewed = 0;
    int index = hash(newItem.key, viewed, msize);
    KeySpace *item;
    while (viewed < msize){
        item = ks + index;
        if (item -> busy == 0){
            *item = newItem;
            break;
        }
        viewed++;
        index = hash(newItem.key, viewed, msize);
    }
}

Status expandTable(Table *table){
    KeySpace *firstItem = (table -> ks), *item;
    size_t newMsize = getNewMsize(table -> msize);
    KeySpace *newKS = (KeySpace*)calloc(newMsize, sizeof(KeySpace));
    if (newKS == NULL){ return ERR_MEMORY_ERROR; }
    for (int index = 0; index < (table -> msize); index++){
        item = firstItem + index;
        if (item -> busy == 1){
            putItem(newKS, *item, newMsize);
        }
    }
    free(table -> ks);
    table -> ks = newKS;
    table -> msize = newMsize;
    return OK;
}

Status insertItem(Table *table, unsigned int key, char *info){
    if (table -> csize == (table -> msize)){
        Status err = expandTable(table);
        if (err != OK){
            return err;
        }
    }
    int viewed = 0;
    int index = hash(key, viewed, table -> msize);
    KeySpace *firstItem = (table -> ks);
    KeySpace *item;
    int indexForInsert = -1, indexForDelete = -1, delta = 1;
    while (viewed < (table -> msize)){
        item = firstItem + index;
        if (item -> busy != 1 || item -> key == key){
            indexForInsert = (indexForInsert == -1)?index:indexForInsert;
            if (item -> busy == 1){
                indexForDelete = index;
            }
            if (item -> busy != -1){
                break;
            }
        }
        viewed++;
        index = hash(key, viewed, table -> msize);
    }
    if (indexForInsert == -1){
        return ERR_NOT_FOUND;
    }
    char *newInfo = (char*)malloc(sizeof(char)*(strlen(info) + 1));
    if (newInfo == NULL){ return ERR_MEMORY_ERROR; }
    strcpy(newInfo, info);
    if (indexForDelete != -1){
        item = firstItem + indexForDelete;
        free(item -> info);
        item -> busy = -1;
        delta = 0;
    }
    item = firstItem + indexForInsert;
    item -> busy = 1;
    item -> key = key;
    item -> info = newInfo;
    table -> csize += delta;
    return OK;
}

Status delItem(Table *table, unsigned int key){
    KeySpace *firstItem = (table -> ks);
    int viewed = 0;
    int index = hash(key, viewed, table -> msize);
    KeySpace *item = firstItem + index;
    int deleted = 0;
    while ((item -> busy != 0) && (viewed < (table -> msize))){
        if ((item -> busy == 1) && (item -> key == key)){
            free(item -> info);
            item -> busy = -1;
            table -> csize -= 1;
            deleted = 1;
            break;
        }
        viewed++;
        index = hash(key, viewed, table -> msize);
        item = firstItem + index;
    }
    if (deleted == 0){
        return ERR_NOT_FOUND;
    }
    return OK;
}

void freeTable(Table *table){
    KeySpace *lastItem = (table -> ks + (table -> msize));
    for (KeySpace *item = table -> ks; item != lastItem; item++){
        if (item -> busy == 1){ free(item -> info); }
    }
    free(table -> ks);
    free(table);
}
