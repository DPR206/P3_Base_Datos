#include <stdio.h>
#include <stdlib.h>

typedef struct
{
  size_t size;
  size_t offset;
} Indexdeletedbook;

typedef struct
{
  Indexdeletedbook **indexdeleted_array;
  size_t used;
  size_t size;
} Array_indexdeleted;

#define BESTFIT 0  /* Insertar en el hueco más pequeño que sea válido */
#define WORSTFIT 1 /* Insertar en el hueco más grande que exista */
#define FIRSTFIT 2 /* Insertar en el primer hueco valido */

#define HEADER 8 /* Tamaño de size_t */

Array_indexdeleted *initArrayDeleted(size_t initialSize);
void insertArrayDeleted(Array_indexdeleted *array, Indexdeletedbook *index, int mode);
int deleteArrayDeleted(Array_indexdeleted *array, int bookId);
void freeArrayDeleted(Array_indexdeleted *array);
void printArrayDeleted(Array_indexdeleted *array);

void findgapDeleted(Array_indexdeleted *array, size_t size, size_t *offset, int mode);

Indexdeletedbook *create_Indexdeleted(size_t offset, size_t size);
void free_Indexdeleted(Indexdeletedbook *indexdeleted);