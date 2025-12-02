#include <stdio.h>
#include <stdlib.h>

#include "index.h"
#include "deleted.h"

void add(Array_index *indexarray, Array_indexdeleted *deletedarray,  char *index_file, int key, size_t size, char *info, int mode);
void find(Array_index *indexarray, char *index_file, int bookId);
void del(Array_index *indexarray, Array_indexdeleted *indexdeletedarray, char *indexdeleted_file, int bookId, int mode);