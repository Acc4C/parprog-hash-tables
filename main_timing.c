#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "table.h"
#include "myio.h"

Status generateKeys(char **keys, size_t count){
    int length;
    char *key;
    for (int i = 0; i < count; i++){
        length = 3 + rand()%8;
        *(keys + i) = (char*)calloc(length + 1, sizeof(char));
        key = *(keys + i);
        if (key == NULL){
            for (i -= 1; i >= 0; i--){
                free(*(keys + i));
            }
            return ERR_MEMORY_ERROR;
        }
        for (int j = 0; j < length; j++){
            *(key + j) = 65 + rand()%58;
        }
    }
    return OK;
}

void clearStringArray(char **keys, size_t count){
    for (int i = 0; i < count; i++){
        free(*(keys + i));
    }
}

Status benchmarkInsert(size_t msize, double *fillInTime){
    Status err;
    double start, end, sum = 0;
    Table *table = createTable(getNewMsize(msize));
    if (table == NULL){
        return ERR_MEMORY_ERROR;
    }
    char **keys = (char**)malloc(msize*sizeof(char*));
    if (keys == NULL){
        freeTable(table);
        return ERR_MEMORY_ERROR;
    }
    for (int i = 0; i < 100; i++){
        err = generateKeys(keys, msize);
        if (err != OK){
            freeTable(table);
            free(keys);
            return err;
        }
        start = omp_get_wtime();
        for (int j = 0; j < msize; j++){
            err = insertItem(table, *(keys + j), *(keys + j));
            if (err != OK){
                freeTable(table);
                clearStringArray(keys, msize);
                free(keys);
                return err;
            }
        }
        end = omp_get_wtime();
        sum += end - start;
        clearTable(table);
        clearStringArray(keys, msize);
    }
    *fillInTime = sum / 100;
    freeTable(table);
    free(keys);
    return OK;
}

Status benchmarkSearch(size_t msize, double *searchTime){
    Status err;
    double start, end, sum = 0;
    Table *table = createTable(getNewMsize(msize));
    if (table == NULL){
        return ERR_MEMORY_ERROR;
    }
    char **keys = (char**)malloc(msize*sizeof(char*));
    if (keys == NULL){
        freeTable(table);
        return ERR_MEMORY_ERROR;
    }
    err = generateKeys(keys, msize);
    if (err != OK){
        freeTable(table);
        free(keys);
        return err;
    }
    for (int j = 0; j < msize/2; j++){
        err = insertItem(table, *(keys + j), *(keys + j));
        if (err != OK){
            freeTable(table);
            clearStringArray(keys, msize);
            free(keys);
            return err;
        }
    }
    char **vals = (char**)calloc(msize, sizeof(char*));
    if (vals == NULL){
        freeTable(table);
        clearStringArray(keys, msize);
        free(keys);
        return ERR_MEMORY_ERROR;
    }
    start = omp_get_wtime();
    for (int i = 0; i < msize; i++){
        *(vals + i) = getItem(table, *(keys + i), &err);
        if (err == ERR_MEMORY_ERROR){
            freeTable(table);
            clearStringArray(keys, msize);
            free(keys);
            clearStringArray(vals, msize);
            free(vals);
            return err;
        }
    }
    end = omp_get_wtime();
    *searchTime = (end - start)/msize;
    freeTable(table);
    clearStringArray(keys, msize);
    free(keys);
    clearStringArray(vals, msize);
    free(vals);
    return OK;
}

Status benchmarkTable(size_t msize, double *fillInTime, double *searchTime){
    Status err = benchmarkInsert(msize, fillInTime);
    if (err != OK){
        return err;
    }
    return benchmarkSearch(msize, searchTime);
}

int main(){
    Status err;
    double *tableFillInTimes = malloc(50*sizeof(double));
    if (tableFillInTimes == NULL){
        printError(ERR_MEMORY_ERROR);
        return 0;
    }
    double *tableSearchTimes = malloc(50*sizeof(double));
    if (tableSearchTimes == NULL){
        free(tableFillInTimes);
        printError(ERR_MEMORY_ERROR);
        return 0;
    }
    for (size_t msize = 10000; msize <= 500000; msize += 10000){
        err = benchmarkTable(msize, tableFillInTimes + msize/10000 - 1, tableSearchTimes + msize/10000 - 1);
        if (err != OK){
            free(tableFillInTimes);
            free(tableSearchTimes);
            printError(err);
            return 0;
        }
        printf("step %d passed\n", (int)msize/10000);
    }
    printf("fill in table benchmarks:\n");
    printf("element count   time\n");
    for (int i = 1; i < 51; i++){
        printf("%-15d %.3fs\n", i*10000, *(tableFillInTimes + i - 1));
    }
    printf("search in table benchmarks:\n");
    printf("element count   time\n");
    for (int i = 1; i < 51; i++){
        printf("%-15d %.3fmcs\n", i*10000, *(tableSearchTimes + i - 1)*1000000);
    }
    free(tableFillInTimes);
    free(tableSearchTimes);
    return 0;
}
