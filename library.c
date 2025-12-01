#include <stdio.h>
#include <stdlib.h>

#include "library.h"

#define N_CMD 6
char *comand[N_CMD] = {"add", "find", "del", "exit", "printInd", "printLst"};

int main() {
    Array_index *indexarray = NULL;
    Array_indexdeleted *indexdeletedarray = NULL;
    char index_file[] = "index.txt";
    char deleted_file[] = "deleted.txt";
    int i;

    indexarray = initArray(2);

    add(indexarray, index_file, 3, 22, 46, "12345|978-2-12345680-3|El Quijote|catedra");
    add(indexarray, index_file, 1, 42, 36, "12346|978-2-12345680-4|Hola|catedra");
    add(indexarray, index_file, 2, 0, 42, "12345|978-2-12345680-3|El Quijote|catedra");

    for(i=0; i<3; i++){
    fprintf(stdout, "%d, %ld, %ld\n", indexarray->index_array[i]->key, indexarray->index_array[i]->offset, indexarray->index_array[i]->size);
    }

    del(indexarray, indexdeletedarray, deleted_file, 2);
    del(indexarray, indexdeletedarray, deleted_file, 4);

    for(i=0; i<2; i++){
    fprintf(stdout, "%d, %ld, %ld\n", indexarray->index_array[i]->key, indexarray->index_array[i]->offset, indexarray->index_array[i]->size);
    }

    freeArray(indexarray);
    free(indexarray);

    return 0;
}