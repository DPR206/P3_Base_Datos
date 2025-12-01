#include <stdio.h>
#include <stdlib.h>

typedef struct
{
  int key; /*book isbn*/
  long int offset; /*book is stored in disk in this position*/
  size_t size; /*book recorded size. This is a redundant field that helps in the implementation*/
} Indexbook;

typedef struct
{
  size_t size;
  size_t offset;
} Indexdeletedbook;

typedef struct
{
  Indexbook **index_array;
  size_t used;
  size_t size;
} Array_index;

typedef struct
{
  Indexdeletedbook **indexdeleted_array;
  size_t used;
  size_t size;
} Array_indexdeleted;

#define BESTFIT 0
#define WORSTFIT 1
#define FIRSTFIT 2

Indexbook *create_Indexbook(int key, long int offset, size_t size);
void free_Indexbook(Indexbook *indexbook);

Indexdeletedbook *create_Indexdeleted(size_t offset, size_t size);
void free_Indexdeleted(Indexdeletedbook *indexdeleted);

Indexbook *find_index_fromId(Array_index *array, int bookId, int beginning, int end, int *pos);
int bin_search(Array_index *array, int F, int L, Indexbook *index);

Array_index *initArray(size_t initialSize);
void insertArray(Array_index *ai, Indexbook *index);
int deleteArray(Array_index *ai, int bookId);
void freeArray(Array_index *ai);

Array_indexdeleted *initArrayDeleted(size_t initialSize);
void insertArrayDeleted(Array_indexdeleted *array, Indexdeletedbook *index);
int deleteArrayDeleted(Array_indexdeleted *array, int bookId);
void freeArrayDeleted(Array_indexdeleted *array);

void add(Array_index *indexarray, char *index_file, int key, long int offset, size_t size, char *info);
void find(Array_index *indexarray, char *index_file, int bookId);
void del(Array_index *indexarray, Array_indexdeleted *indexdeletedarray, char *indexdeleted_file, int bookId);