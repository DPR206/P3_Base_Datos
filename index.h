#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct
{
  int key; /*book id*/
  size_t offset; /*book is stored in disk in this position*/
  size_t size; /*book recorded size. This is a redundant field that helps in the implementation*/
} Indexbook;

typedef struct
{
  Indexbook **index_array;
  size_t used;
  size_t size;
} Array_index;

Indexbook *create_Indexbook(int key, long int offset, size_t size);
void free_Indexbook(Indexbook *indexbook);

Indexbook *find_index_fromId(Array_index *array, int bookId, int beginning, int end, int *pos);
int bin_search(Array_index *array, int F, int L, Indexbook *index);

Array_index *initArray(size_t initialSize);
void insertArray(Array_index *ai, Indexbook *index);
int deleteArray(Array_index *ai, int bookId, Indexbook **indexdeleted);
void freeArray(Array_index *ai);
void printArray(Array_index *array);