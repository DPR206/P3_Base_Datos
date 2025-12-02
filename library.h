#include <stdio.h>
#include <stdlib.h>

#include "index.h"
#include "deleted.h"

void add(Array_index *indexarray, char *index_file, int key, long int offset, size_t size, char *info);
void find(Array_index *indexarray, char *index_file, int bookId);
void del(Array_index *indexarray, Array_indexdeleted *indexdeletedarray, char *indexdeleted_file, int bookId);