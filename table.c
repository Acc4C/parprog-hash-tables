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

KeySpace *getItem(Table* table, unsigned int key, int release){
    int viewed = 0;
    int index = hash(key, viewed, table -> msize);
    KeySpace *firstItem = (table -> ks);
    while (((firstItem + index) -> busy != 0) && (viewed < (table -> msize))){
        if (((firstItem + index) -> busy == 1) && ((firstItem + index) -> key == key) && ((firstItem + index) -> release == release)){
            return firstItem + index;
        }
        viewed++;
        index = hash(key, viewed, table -> msize);
    }
    return NULL;
}

int getMaxVersion(Table* table, unsigned int key){
    int viewed = 0, version = 0;
    int index = hash(key, viewed, table -> msize);
    KeySpace *firstItem = (table -> ks);
    KeySpace *item = firstItem + index;
    while ((item -> busy != 0) && (viewed < (table -> msize))){
        if ((item -> busy == 1) && (item -> key == key)){
            version = (item -> release > version)?(item -> release):(version);
        }
        viewed++;
        index = hash(key, viewed, table -> msize);
        item = firstItem + index;
    }
    return version;
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

int insertItem(Table *table, unsigned int key, char *info){
    int viewed = 0;
    int index = hash(key, viewed, table -> msize);
    KeySpace *firstItem = (table -> ks);
    KeySpace *item;
    while (viewed < (table -> msize)){
        item = firstItem + index;
        if (item -> busy != 1){
            item -> release = getMaxVersion(table, key) + 1;
            item -> busy = 1;
            item -> key = key;
            char *newInfo = (char*)malloc(sizeof(char)*(strlen(info) + 1));
            if (newInfo == NULL){ return ERR_MEMORY_ERROR; }
            strcpy(newInfo, info);
            item -> info = newInfo;
            table -> csize += 1;
            break;
        }
        viewed++;
        index = hash(key, viewed, table -> msize);
    }
    if (table -> csize < (table -> msize)){
        return OK;
    }
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

void freeKeySpaceArray(KeySpace *array){
    for (KeySpace *item = array; item -> info != NULL; item++){ free(item -> info); }
    free(array);
}

KeySpace *getItemsByKey(Table *table, unsigned int key, Status *err){
    KeySpace *firstItem = (table -> ks);
    KeySpace *itemArray = (KeySpace*)malloc(sizeof(KeySpace)*(table -> csize + 1));
    if (itemArray == NULL){
        *err = ERR_MEMORY_ERROR;
        return NULL;
    }
    int length = 0, viewed = 0;
    int index = hash(key, viewed, table -> msize);
    KeySpace *item = firstItem + index;
    while ((item -> busy != 0) && (viewed < (table -> msize))){
        if ((item -> busy == 1) && (item -> key == key)){
            itemArray[length] = *item;
            itemArray[length].info = (char*)malloc(sizeof(char)*(strlen(itemArray[length].info) + 1));
            if (itemArray[length].info == NULL){
                for (; length > 0; length--){ free(itemArray[length - 1].info); }
                free(itemArray);
                *err = ERR_MEMORY_ERROR;
                return NULL;
            }
            strcpy(itemArray[length].info, item -> info);
            length++;
        }
        viewed++;
        index = hash(key, viewed, table -> msize);
        item = firstItem + index;
    }
    if (length == 0){
        *err = ERR_NOT_FOUND_BY_KEY;
        free(itemArray);
        return NULL;
    }
    itemArray[length].info = NULL;
    itemArray = (KeySpace*)realloc(itemArray, sizeof(KeySpace)*(length + 1));
    if (itemArray == NULL){
        *err = ERR_MEMORY_ERROR;
        return NULL;
    }
    *err = OK;
    return itemArray;
}

KeySpace *getItemsByKeyAndVersion(Table *table, unsigned int key, int version, Status *err){
    if (version == 0){ return getItemsByKey(table, key, err); }
    KeySpace* itemArray = getItemsByKey(table, key, err);
    if (*err){ return NULL; }
    for (KeySpace* item = itemArray; item -> info != NULL; item++){
        if (item -> release == version){
            for (KeySpace* exItem = item + 1; exItem -> info != NULL; exItem++){ free(exItem -> info); }
            for (KeySpace* exItem = itemArray; exItem != item; exItem++){ free(exItem -> info); }
            itemArray[0] = *item;
            itemArray[1].info = NULL;
            itemArray = (KeySpace*)realloc(itemArray, sizeof(KeySpace)*2);
            if (itemArray == NULL){
                *err = ERR_MEMORY_ERROR;
                return NULL;
            }
            *err = OK;
            return itemArray;
        }
    }
    for (KeySpace *item = itemArray; item -> info != NULL; item++){ free(item -> info); }
    free(itemArray);
    *err = ERR_NOT_FOUND_BY_KEY_AND_VERSION;
    return NULL;
}

Status delItems(Table *table, unsigned int key){
    KeySpace *firstItem = (table -> ks);
    int viewed = 0;
    int index = hash(key, viewed, table -> msize);
    KeySpace *item = firstItem + index;
    if (item -> busy == 0){
        return ERR_NOT_FOUND_BY_KEY;
    }
    while ((item -> busy != 0) && (viewed < (table -> msize))){
        if ((item -> busy == 1) && (item -> key == key)){
            free(item -> info);
            item -> busy = -1;
            table -> csize -= 1;
        }
        viewed++;
        index = hash(key, viewed, table -> msize);
        item = firstItem + index;
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
