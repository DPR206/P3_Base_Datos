#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct
{
  int key; /* 4 bytes*/
  size_t offset; /* 8 bytes */
  size_t size; /* 8 bytes */
} Indexbook;

typedef struct
{
  Indexbook **index_array;
  size_t used;
  size_t size;
} Array_index;

typedef enum {OK, ERR} Status;

Indexbook *create_Indexbook(int key, long int offset, size_t size);
void free_Indexbook(Indexbook *indexbook);

Indexbook *find_index_fromId(Array_index *array, int bookId, int beginning, int end, int *pos);

Array_index *initArray(size_t initialSize);
Status insertArray(Array_index *ai, Indexbook *index);
Status deleteArray(Array_index *ai, int bookId, Indexbook **indexdeleted);
void freeArray(Array_index *ai);
Status printArray(Array_index *array);
