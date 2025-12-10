#include <stdio.h>
#include <stdlib.h>

#include "deleted.h"

#define MAX_LEN 256
#define INDEXDATA_SIZE 20
#define DELETEDDATA_SIZE 16

#define S_SIZET 8 /* Size of size_t */
#define S_INT 4 /* Size of int */
#define ISBN 16 /* Size of isbn */

Status comand_add(Array_index *indexarray, Array_indexdeleted *deletedarray, FILE *fdata, char *info, int mode);
Status comand_find(Array_index *indexarray, FILE *fdata, int bookId);
Status comand_del(Array_index *indexarray, Array_indexdeleted *indexdeletedarray, int bookId, int mode);
void comand_exit(Array_index *indexarray, Array_indexdeleted *indexdeletedarray, FILE *fdata, FILE *findex, FILE *fdeleted, int mode);
Status comand_printInd(Array_index *indexarray);
Status comand_printLst(Array_indexdeleted *deletedarray);
Status comand_printRec(Array_index *indexarray, FILE *fdata);
Status comand_load(Array_index *indexarray, Array_indexdeleted *deletedarray, FILE *findex, FILE *fdeleted, int mode);

Status findgapDeleted(Array_index *indexarray, Array_indexdeleted *array, size_t size, long int *offset, int mode);