#include <stdio.h>
#include <stdlib.h>

typedef enum {ERROR = -1, OK = 0}Status;

typedef struct
{
  int key; /*book isbn*/
  long int offset; /*book is stored in disk in this position*/
  size_t size; /*book recorded size. This is a redundant field that helps in the implementation*/
} Indexbook;

typedef struct
{
  size_t register_size;
  size_t offset;
} Indexdeletedbook;

typedef struct
{
  Indexbook **index_array;
  size_t used;
  size_t size;
} Array_index;

#define BESTFIT 0
#define WORSTFIT 1
#define FIRSTFIT 2

Indexbook *create_Indexbook(int key, long int offset, size_t size);
Indexbook *find_index_fromId(Array_index *array, int bookId, int beginning, int end, int *pos);
int bin_search_find(Indexbook **array, Indexbook *index_search, int beginning, int end);
int bin_search_delete(Indexbook **array, Indexbook *index_search, int beginning, int end);
void free_Indexbook(Indexbook *indexbook);

void initArray(Array_index *ai, size_t initalSize);
Status insertArray(Array_index *ai, Indexbook *index);
Status deletetArray(Array_index *ai, Indexbook *index);
void freeArray(Array_index *ai);

void find(array *indexarray, int bookId);
void del(array *indexarray, array *indexdeletedarray, int bookId);
