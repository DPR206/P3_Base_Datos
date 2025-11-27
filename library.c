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

    indexarray = initArray(2);

    add(indexarray, index_file, 1, 0, 42, "12345|978-2-12345680-3|El Quijote|catedra");
    add(indexarray, index_file, 2, 42, 36, "12346|978-2-12345680-4|Hola|catedra");
    add(indexarray, index_file, 1, 0, 42, "12345|978-2-12345680-3|El Quijote|catedra");

    fprintf(stdout, "%d, %ld, %ld\n", indexarray->index_array[0]->key, indexarray->index_array[0]->offset, indexarray->index_array[0]->size);
    fprintf(stdout, "%d, %ld, %ld\n", indexarray->index_array[1]->key, indexarray->index_array[1]->offset, indexarray->index_array[1]->size);

    freeArray(indexarray);
    free(indexarray);

    return 0;
}