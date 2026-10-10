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
    *fillInTime = sum/100;
    freeTable(table);
    free(keys);
    return OK;
}

typedef struct{
    double successful;
    double unsuccessful;
} searchTime;

Status benchmarkSearchByElementCount(size_t msize, searchTime *searchTimePtr){
    Status err;
    double start, end, sumSuccessful = 0, sumUnsuccessful = 0, plug = 0;
    Table *table = createTable(getNewMsize(2*msize));
    int searchCount = (msize < 100000)?msize:100000;
    if (table == NULL){
        return ERR_MEMORY_ERROR;
    }
    int *keys = (int*)malloc(msize*sizeof(int));
    if (keys == NULL){
        freeTable(table);
        return ERR_MEMORY_ERROR;
    }
    for (int k = 0; k < 10; k++){
        generateKeys(keys, msize);
        for (int j = 0; j < msize; j++){
            err = insertItem(table, keys[j], keys[j]);
            if (err != OK){
                freeTable(table);
                free(keys);
                return err;
            }
        }
        start = omp_get_wtime();
        for (int i = 0; i < searchCount; i++){
            plug += getItem(table, keys[i], &err);
        }
        end = omp_get_wtime();
        sumSuccessful += end - start;
        generateKeys(keys, searchCount);
        start = omp_get_wtime();
        for (int i = 0; i < searchCount; i++){
            plug += getItem(table, keys[i], &err);
        }
        end = omp_get_wtime();
        sumUnsuccessful += end - start;
        clearTable(table);
    }
    searchTimePtr -> successful = sumSuccessful/10/searchCount;
    searchTimePtr -> unsuccessful = sumUnsuccessful/10/searchCount;
    freeTable(table);
    free(keys);
    return OK;
}

Status benchmarkTable(size_t msize, double *fillInTime, searchTime *tableSearchTimeByElementCount){
    Status err = benchmarkInsert(msize, fillInTime);
    if (err != OK){
        return err;
    }
    return benchmarkSearchByElementCount(msize, tableSearchTimeByElementCount);
}

int main(){
    Status err;
    double *tableFillInTimes = (double *)malloc(10*sizeof(double));
    if (tableFillInTimes == NULL){
        printError(ERR_MEMORY_ERROR);
        return 0;
    }
    searchTime *tableSearchTimesByElementCount = (searchTime *)malloc(10*sizeof(searchTime));
    if (tableSearchTimesByElementCount == NULL){
        free(tableFillInTimes);
        printError(ERR_MEMORY_ERROR);
        return 0;
    }
    for (int i = 1; i <= 10; i++){
        size_t msize = 500000*i;
        err = benchmarkTable(msize, tableFillInTimes + i - 1, tableSearchTimesByElementCount + i - 1);
        if (err != OK){
            free(tableFillInTimes);
            free(tableSearchTimesByElementCount);
            printError(err);
            return 0;
        }
        printf("step %d passed\n", i);
    }
    printf("\nfill in table benchmarks:\n");
    printf("element count   time\n");
    for (int i = 1; i <= 10; i++){
        printf("%-15d %.3fms\n", i*500000, tableFillInTimes[i - 1]*1000);
    }
    printf("\nsearch in table by element count benchmarks:\n");
    printf("element count   successful / unsuccessful time\n");
    for (int i = 1; i <= 10; i++){
        printf("%-15d %.3fmcs / %.3fmcs\n", i*500000, tableSearchTimesByElementCount[i - 1].successful*1000000, tableSearchTimesByElementCount[i - 1].unsuccessful*1000000);
    }
    free(tableFillInTimes);
    free(tableSearchTimesByElementCount);
    return 0;
}
