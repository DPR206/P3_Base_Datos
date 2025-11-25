#include <stdio.h>
#include <stdlib.h>

typedef struct
{
  int key; /*book isbn*/
  long int offset; /*book is stored in disk in this position*/
  size_t size; /*book recorded size. This is a redundant
                 field that helps in the implementation*/
}Indexbook;


typedef struct
{
  Indexbook *index_array;
  size_t used;
  size_t size;
} Array_index;

Indexbook *create_Indexbook(int key, long int offset, size_t size);
int search_register(Indexbook *array, Indexbook index_search, int beginning, int end);
void free_Indexbook(Indexbook *indexbook);

void initArray(Array_index *ai, size_t initalSize);
void insertArray(Array_index *ai, Indexbook index);
void deletetArray(Array_index *ai, Indexbook index);
void freeArray(Array_index *ai);