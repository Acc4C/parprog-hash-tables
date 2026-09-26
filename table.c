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
            if (newInfo == NULL){ return 1; }
            strcpy(newInfo, info);
            item -> info = newInfo;
            table -> csize += 1;
            break;
        }
        viewed++;
        index = hash(key, viewed, table -> msize);
    }
    if (table -> csize < (table -> msize)){
        return 0;
    }
    size_t newMsize = getNewMsize(table -> msize);
    KeySpace *newKS = (KeySpace*)calloc(newMsize, sizeof(KeySpace));
    if (newKS == NULL){ return 1; }
    for (int index = 0; index < (table -> msize); index++){
        item = firstItem + index;
        if (item -> busy == 1){
            putItem(newKS, *item, newMsize);
        }
    }
    free(table -> ks);
    table -> ks = newKS;
    table -> msize = newMsize;
    return 0;
}

void freeKeySpaceArray(KeySpace *array){
    for (KeySpace *item = array; item -> info != NULL; item++){ free(item -> info); }
    free(array);
}

KeySpace *getItemsByKey(Table *table, unsigned int key, int *err){
    KeySpace *firstItem = (table -> ks);
    KeySpace *itemArray = (KeySpace*)malloc(sizeof(KeySpace)*(table -> csize + 1));
    if (itemArray == NULL){
        *err = 1;
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
                *err = 1;
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
        *err = 3;
        free(itemArray);
        return NULL;
    }
    itemArray[length].info = NULL;
    itemArray = (KeySpace*)realloc(itemArray, sizeof(KeySpace)*(length + 1));
    if (itemArray == NULL){
        *err = 1;
        return NULL;
    }
    *err = 0;
    return itemArray;
}

KeySpace *getItemsByKeyAndVersion(Table *table, unsigned int key, int version, int *err){
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
                *err = 1;
                return NULL;
            }
            *err = 0;
            return itemArray;
        }
    }
    for (KeySpace *item = itemArray; item -> info != NULL; item++){ free(item -> info); }
    free(itemArray);
    *err = 2;
    return NULL;
}

Table *importFromFile(char *path, int *err){
    FILE *file = fopen(path, "rb");
    int length;
    size_t msize;
    if (file == NULL){
        *err = 5;
        return NULL;
    }
    if (fread(&msize, sizeof(size_t), 1, file) != 1){
        *err = 4;
        fclose(file);
        return NULL;
    }
    Table *newTable = createTable(msize);
    if (newTable == NULL){
        *err = 1;
        fclose(file);
        return NULL;
    }
    if (fread(&(newTable -> csize), sizeof(size_t), 1, file) != 1){
        *err = 4;
        fclose(file);
        freeTable(newTable);
        return NULL;
    }
    KeySpace* ks = (newTable -> ks);
    if (fread(newTable -> ks, sizeof(KeySpace), newTable -> msize, file) != (newTable -> msize)){
        for (int index = 0; index < (newTable -> msize); index++){ (ks + index) -> busy = 0; }
        fclose(file);
        freeTable(newTable);
        *err = 4;
        return NULL;
    }
    for (int index = 0; index < (newTable -> msize); index++){
        if ((ks + index) -> busy == 1){
            if (fread(&length, sizeof(int), 1, file) != 1){
                for (; index < (newTable -> msize); index++){ (ks + index) -> busy = 0; }
                fclose(file);
                freeTable(newTable);
                *err = 4;
                return NULL;
            }
            (ks + index) -> info = (char*)malloc(sizeof(char)*length);
            if ((ks + index) -> info == NULL){
                for (; index < (newTable -> msize); index++){ (ks + index) -> busy = 0; }
                fclose(file);
                freeTable(newTable);
                *err = 1;
                return NULL;
            }
            if (fread((ks + index) -> info, sizeof(char), length, file) != length){
                for (index += 1; index < (newTable -> msize); index++){ (ks + index) -> busy = 0; }
                fclose(file);
                freeTable(newTable);
                *err = 4;
                return NULL;
            }
        }
    }
    fclose(file);
    return newTable;
}

int exportInFile(Table *table, char *path){
    FILE *file = fopen(path, "wb");
    int length;
    if (file == NULL){ return 5; }
    fwrite(&(table -> msize), sizeof(size_t), 1, file);
    fwrite(&(table -> csize), sizeof(size_t), 1, file);
    fwrite(table -> ks, sizeof(KeySpace), table -> msize, file);
    for (int i = 0; i < table -> msize; i++){
        if ((table -> ks + i) -> busy == 1){
            length = strlen((table -> ks + i) -> info) + 1;
            fwrite(&length, sizeof(int), 1, file);
            fwrite((table -> ks + i) -> info, sizeof(char), length, file);
        }
    }
    fclose(file);
    return 0;
}

int delItems(Table *table, unsigned int key){
    KeySpace *firstItem = (table -> ks);
    int viewed = 0;
    int index = hash(key, viewed, table -> msize);
    KeySpace *item = firstItem + index;
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
    return 0;
}

void freeTable(Table *table){
    KeySpace *lastItem = (table -> ks + (table -> msize));
    for (KeySpace *item = table -> ks; item != lastItem; item++){
        if (item -> busy == 1){ free(item -> info); }
    }
    free(table -> ks);
    free(table);
}
