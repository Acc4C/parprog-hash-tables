#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "table.h"
#include "myio.h"

void generateKeys(int *keys, size_t count){
    for (int i = 0; i < count; i++){
        keys[i] = rand();
    }
}

Status benchmarkInsert(size_t msize, double *fillInTime){
    Status err;
    double start, end, sum = 0;
    Table *table = createTable(getNewMsize(2*msize));
    if (table == NULL){
        return ERR_MEMORY_ERROR;
    }
    int *keys = (int*)malloc(msize*sizeof(int));
    if (keys == NULL){
        freeTable(table);
        return ERR_MEMORY_ERROR;
    }
    for (int i = 0; i < 100; i++){
        generateKeys(keys, msize);
        start = omp_get_wtime();
        for (int j = 0; j < msize; j++){
            err = insertItem(table, keys[j], keys[j]);
            if (err != OK){
                freeTable(table);
                free(keys);
                return err;
            }
        }
        end = omp_get_wtime();
        sum += end - start;
        clearTable(table);
    }
    *fillInTime = sum * 10;
    freeTable(table);
    free(keys);
    return OK;
}

Status benchmarkSearch(size_t msize, double *searchTime){
    Status err;
    double start, end, sum, plug = 0;
    msize *= 2;
    Table *table = createTable(getNewMsize(2*msize));
    if (table == NULL){
        return ERR_MEMORY_ERROR;
    }
    int *keys = (int*)malloc(msize*sizeof(int));
    if (keys == NULL){
        freeTable(table);
        return ERR_MEMORY_ERROR;
    }
    for (int k = 0; k < 100; k++){
        generateKeys(keys, msize);
        for (int j = 0; j < msize/2; j++){
            err = insertItem(table, keys[j], keys[j]);
            if (err != OK){
                freeTable(table);
                free(keys);
                return err;
            }
        }
        start = omp_get_wtime();
        for (int i = 0; i < msize; i++){
            plug += getItem(table, keys[i], &err);
            if (err == ERR_MEMORY_ERROR){
                freeTable(table);
                free(keys);
                return err;
            }
        }
        end = omp_get_wtime();
        sum += end - start;
        clearTable(table);
    }
    *searchTime = sum/100/msize;
    freeTable(table);
    free(keys);
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
    printf("\nfill in table benchmarks:\n");
    printf("element count   time\n");
    for (int i = 1; i < 51; i++){
        printf("%-15d %.3fms\n", i*10000, tableFillInTimes[i - 1]);
    }
    printf("\nsearch in table benchmarks:\n");
    printf("element count   time\n");
    for (int i = 1; i < 51; i++){
        printf("%-15d %.3fmcs\n", i*10000, tableSearchTimes[i - 1]*1000000);
    }
    free(tableFillInTimes);
    free(tableSearchTimes);
    return 0;
}
