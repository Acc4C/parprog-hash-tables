#include <stdio.h>
#include <stdlib.h>
#include "table.h"
#include "myio.h"
#include "dialog.h"

int isSimple(int n){
    for (int i = 2; i*i <= n; i++){
        if (n%i == 0){ return 0; }
    }
    return 1;
}

int main(){
    int choise;
    Status err;
    size_t msize;
    printf("Введите количество ячеек таблицы: ");
    err = getSize_t(&msize);
    while (err == ERR_INCORRECT_INPUT || (err == OK && (msize < 1 || !isSimple(msize)))){
        if (err == OK && !isSimple(msize)){ printError(ERR_NOT_PRIME_NUMBER); }
        else { printError(ERR_INCORRECT_INPUT); }
        printf("Введите количество ячеек таблицы: ");
        err = getSize_t(&msize);
    }
    if (err == END_OF_FILE){
        return 0;
    }
    Table *table = createTable(msize);
    if (table == NULL){
        printError(ERR_MEMORY_ERROR);
        return 0;
    }
    while (1){
        printf("Выберите действие:\n");
        printf("  1) Вставить элемент в таблицу\n");
        printf("  2) Удалить элементы по ключу\n");
        printf("  3) Найти элементы по ключу\n");
        printf("  4) Вывести таблицу\n");
        printf("  5) Импортировать данные из бинарного файла\n");
        printf("  6) Экспортировать данные в бинарный файл\n");
        printf("  7) Найти элементы по ключу и номеру версии\n");
        printf("  8) Выход\n");
        printf("Введите цифру: ");
        err = getInt(&choise);
        if (err == END_OF_FILE){
            break;
        } else if (err != OK || choise > 8 || choise < 1){
            printError(ERR_INCORRECT_INPUT);
            printf("\n");
            continue;
        }
        switch (choise){
            case 1:
                err = dialogInsertItem(table);
                break;
            case 2:
                err = dialogDelItems(table);
                break;
            case 3:
                err = dialogGetItemsByKey(table);
                break;
            case 4:
                printTable(table);
                err = OK;
                break;
            case 5:
                err = dialogImportFromFile(&table);
                break;
            case 6:
                err = dialogExportInFile(table);
                break;
            case 7:
                err = dialogGetItemsByKeyAndVersion(table);
                break;
            case 8:
                freeTable(table);
                return 0;
            default:
                err = ERR_INCORRECT_INPUT;
        }
        printError(err);
        printf("\n");
        if (err == ERR_MEMORY_ERROR || err == END_OF_FILE){ break; }
    }
    freeTable(table);
    return 0;
}
