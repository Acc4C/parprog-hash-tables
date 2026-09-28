#include <stdlib.h>
#include <string.h>
#include "table.h"
#include "myio.h"

unsigned int helper(char *key){
    unsigned int res = 1;
    for (char *str = key; *str != 0; str++){
        res = res * 31 + (unsigned char)(*str);
    }
    return res;
}

int hash1(char *key, size_t msize){
    return helper(key)%msize;
}

int hash2(char *key, size_t msize){
    unsigned int helperVal = helper(key);
    return 1 + (helperVal*helperVal)%(msize - 1);
}

int hash(char *key, int i, size_t msize){
    return (hash1(key, msize) + i*hash2(key, msize))%msize;
}

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

char *getItem(Table* table, char *key, Status *err){
    int viewed = 0;
    int index = hash(key, viewed, table -> msize);
    KeySpace *firstItem = (table -> ks);
    KeySpace *item = firstItem + index; 
    while (item -> busy != 0 && viewed < (table -> msize)){
        if (item -> busy == 1 && strcmp(item -> key, key) == 0){
            char *info = item -> info;
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
        item = firstItem + index;
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
        if (flag){
            return i;
        }
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
    if (newKS == NULL){
        return ERR_MEMORY_ERROR;
    }
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

Status insertItem(Table *table, char *key, char *info){
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
        if (item -> busy != 1 || strcmp(item -> key, key) == 0){
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
    char *newKey = (char*)malloc(sizeof(char)*(strlen(key) + 1));
    if (newKey == NULL){
        return ERR_MEMORY_ERROR;
    }
    strcpy(newKey, key);
    char *newInfo = (char*)malloc(sizeof(char)*(strlen(info) + 1));
    if (newInfo == NULL){
        free(newKey);
        return ERR_MEMORY_ERROR;
    }
    strcpy(newInfo, info);
    if (indexForDelete != -1){
        item = firstItem + indexForDelete;
        free(item -> key);
        free(item -> info);
        item -> busy = -1;
        delta = 0;
    }
    item = firstItem + indexForInsert;
    item -> busy = 1;
    item -> key = newKey;
    item -> info = newInfo;
    table -> csize += delta;
    return OK;
}

Status delItem(Table *table, char *key){
    KeySpace *firstItem = (table -> ks);
    int viewed = 0;
    int index = hash(key, viewed, table -> msize);
    KeySpace *item = firstItem + index;
    int deleted = 0;
    while (item -> busy != 0 && viewed < (table -> msize)){
        if (item -> busy == 1 && strcmp(item -> key, key) == 0){
            free(item -> key);
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

void clearTable(Table *table){
    KeySpace *lastItem = (table -> ks + (table -> msize));
    for (KeySpace *item = table -> ks; item != lastItem; item++){
        if (item -> busy == 1){
            free(item -> key);
            free(item -> info);
        }
        item -> busy = 0;
    }
    table -> csize = 0;
}


void freeTable(Table *table){
    KeySpace *lastItem = (table -> ks + (table -> msize));
    for (KeySpace *item = table -> ks; item != lastItem; item++){
        if (item -> busy == 1){
            free(item -> key);
            free(item -> info);
        }
    }
    free(table -> ks);
    free(table);
}
