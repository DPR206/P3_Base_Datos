#include <stdio.h>
#include <stdlib.h>

#include "index.h"
#include "deleted.h"

#define MAX_LEN 256
#define INDEXDATA_SIZE 20
#define DELETEDDATA_SIZE 16

#define S_SIZET 8
#define S_INT 4

void comand_add(Array_index *indexarray, Array_indexdeleted *deletedarray, FILE *fdata, char *info, int mode);
void comand_find(Array_index *indexarray, FILE *fdata, int bookId);
void comand_del(Array_index *indexarray, Array_indexdeleted *indexdeletedarray, int bookId, int mode);
void comand_exit(Array_index *indexarray, Array_indexdeleted *indexdeletedarray, FILE *fdata, FILE *findex, FILE *fdeleted);
void comand_printInd(Array_index *indexarray);
void comand_printLst(Array_indexdeleted *deletedarray);
void comand_printRec(Array_index *indexarray, FILE *fdata);

void findgapDeleted(Array_index *indexarray, Array_indexdeleted *array, size_t size, long int *offset, int mode);