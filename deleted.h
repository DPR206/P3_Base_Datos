#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "index.h"

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

#define BESTFIT 0  /* Insertar en el hueco más pequeño que sea válido */ /* SE ORDENA COMO ENTRA */
#define WORSTFIT 1 /* Insertar en el hueco más grande que exista */
#define FIRSTFIT 2 /* Insertar en el primer hueco valido */

#define HEADER 8 /* Tamaño de size_t */

Array_indexdeleted *initArrayDeleted(size_t initialSize);
Status insertArrayDeleted(Array_indexdeleted *array, Indexdeletedbook *index, int mode);
Status insertDeletedMode(Array_indexdeleted *array, Indexdeletedbook *index);
void freeArrayDeleted(Array_indexdeleted *array);
Status printArrayDeleted(Array_indexdeleted *array);

Indexdeletedbook *create_Indexdeleted(size_t offset, size_t size);
void free_Indexdeleted(Indexdeletedbook *indexdeleted);