#include <stdio.h>
#include <stdlib.h>

#include "library.h"

#define N_CMD 6
char *comand[N_CMD] = {"add", "find", "del", "exit", "printInd", "printLst"};

int main() {
    Array_index *indexarray = NULL;
    Array_indexdeleted *deletedarray = NULL;
    char index_file[] = "index.txt";
    char deleted_file[] = "deleted.txt";
    int i;

    indexarray = initArray(2);
    deletedarray = initArrayDeleted(2);

    for (i=0; i<20; i++){
        if(i%2==0){
            add(indexarray, index_file, i, (i+20), (i+30)%35, "12346|978-2-12345680-4|Hola|catedra");
        }
    }
    for (i=1; i<20; i++){
        if(i%2==1){
            add(indexarray, index_file, i, (i+20), (i+30)%35, "12346|978-2-12345680-4|Hola|catedra");
        }
    }

    printArray(indexarray);
    printArrayDeleted(deletedarray);

    for (i=0; i<20; i++){
        if(i%3==2){
            del(indexarray, deletedarray, deleted_file, i);
        }
    }
    del(indexarray, deletedarray, deleted_file, 2);

    printArray(indexarray);
    printArrayDeleted(deletedarray);

    freeArrayDeleted(deletedarray);
    free(deletedarray);
    freeArray(indexarray);
    free(indexarray);
    return 0;
}