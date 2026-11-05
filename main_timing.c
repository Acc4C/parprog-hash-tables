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

Status defaultInsertBenchmark(size_t tableSize, size_t msize, double *fillInTime){
    Status err;
    double start, end, sum = 0;
    Table *table = createTable(getNewMsize(tableSize));
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

Status benchmarkInsertByElementCount(size_t msize, double *insertTimePtr){
    return defaultInsertBenchmark(2*msize, msize, insertTimePtr);
}

Status benchmarkInsertByLoadFactor(size_t msize, double *insertTimePtr){
    if (msize > 4750000){
        *insertTimePtr = -1;
        return OK;
    }
    return defaultInsertBenchmark(5000000, msize, insertTimePtr);
}

typedef struct{
    double successful;
    double unsuccessful;
} SearchTime;

typedef struct{
    size_t msize;
    double fillInTimeByElementCount;
    double fillInTimeByLoadFactor;
    SearchTime searchTimeByElementCount;
    SearchTime searchTimeByLoadFactor;
} BenchmarkRow;

Status defaultSearchBenchmark(size_t tableSize, size_t msize, SearchTime *searchTimePtr){
    Status err;
    double start, end, sumSuccessful = 0, sumUnsuccessful = 0, plug = 0;
    int searchCount = (msize < 100000)?msize:100000;
    Table *table = createTable(getNewMsize(tableSize));
    if (table == NULL){
        return ERR_MEMORY_ERROR;
    }
    int *keys = (int*)malloc((msize>searchCount?msize:searchCount)*sizeof(int));
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
        int step = msize/searchCount;
        start = omp_get_wtime();
        for (int i = 0; i < searchCount; i ++){
            plug += getItem(table, keys[i*step], &err);
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

Status benchmarkSearchByElementCount(size_t msize, SearchTime *searchTimePtr){
    return defaultSearchBenchmark(2*msize, msize, searchTimePtr);
}

Status benchmarkSearchByLoadFactor(size_t msize, SearchTime *searchTimePtr){
    if (msize > 4750000){
        return OK;
    }
    return defaultSearchBenchmark(5000000, msize, searchTimePtr);
}

Status benchmarkTable(size_t msize, BenchmarkRow *row){
    Status err;
    row->msize = msize;
    err = benchmarkInsertByElementCount(msize, &row->fillInTimeByElementCount);
    if (err != OK){
        return err;
    }
    err = benchmarkInsertByLoadFactor(msize, &row->fillInTimeByLoadFactor);
    if (err != OK){
        return err;
    }
    err = benchmarkSearchByElementCount(msize, &row->searchTimeByElementCount);
    if (err != OK){
        return err;
    }
    err = benchmarkSearchByLoadFactor(msize, &row->searchTimeByLoadFactor);
    return err;
}

int main(){
    Status err;
    BenchmarkRow rows[10];
    for (int i = 1; i <= 10; i++){
        size_t msize = 500000*i;
        err = benchmarkTable(msize, &rows[i - 1]);
        if (err != OK){
            printError(err);
            return 1;
        }
        printf("benchmark with %zu elements passed\n", msize);
    }
    printf("\nfill in table by element count benchmarks:\n");
    printf("element count   time\n");
    for (int i = 0; i < 10; i++){
        printf("%-15zu %.3fms\n", rows[i].msize, rows[i].fillInTimeByElementCount*1000);
    }
    printf("\nfill in table by load factor benchmarks:\n");
    printf("load factor, %%  time\n");
    for (int i = 0; i < 9; i++){
        printf("%-15d %.3fms\n", (i + 1)*10, rows[i].fillInTimeByLoadFactor*1000);
    }
    printf("\nsearch in table by element count benchmarks:\n");
    printf("element count   successful / unsuccessful time\n");
    for (int i = 0; i < 10; i++){
        printf("%-15zu %.3fmcs / %.3fmcs\n", rows[i].msize, rows[i].searchTimeByElementCount.successful*1000000, rows[i].searchTimeByElementCount.unsuccessful*1000000);
    }
    printf("\nsearch in table by load factor benchmarks:\n");
    printf("load factor, %%  successful / unsuccessful time\n");
    for (int i = 0; i < 9; i++){
        printf("%-15d %.3fmcs / %.3fmcs\n", (i + 1)*10, rows[i].searchTimeByLoadFactor.successful*1000000, rows[i].searchTimeByLoadFactor.unsuccessful*1000000);
    }
    return 0;
}
