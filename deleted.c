#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "deleted.h"

/********************************/
/*    FUNCIONES INDEXDELETED    */
/********************************/



Indexdeletedbook *create_Indexdeleted(size_t offset, size_t size){
  Indexdeletedbook *new_index = NULL;

  if (size == 0)
  {
    return NULL;
  }
  
  new_index = (Indexdeletedbook *)calloc(1, sizeof(Indexdeletedbook));

  if (!new_index)
  {
    return NULL;
  }
  
  new_index->offset = offset;
  new_index->size = size;

  return new_index;
}

void free_Indexdeleted(Indexdeletedbook *indexdeleted){
  free(indexdeleted);
  return;
}



/********************************/
/* FUNCIONES ARRAY_INDEXDELETED */
/********************************/



Array_indexdeleted *initArrayDeleted(size_t initialSize){
  Array_indexdeleted *new = NULL;

  if (initialSize == 0) 
  {
    return NULL;
  }
  
  new = (Array_indexdeleted *)calloc(1, sizeof(Array_indexdeleted));

  /*Create initial empty array of size initialSize*/
  new->indexdeleted_array = (Indexdeletedbook **)calloc(initialSize, sizeof(Indexdeletedbook *));
  if (!new->indexdeleted_array)
  {
    free(new);
    return NULL;
  }
  
  new->used = 0;
  new->size = initialSize;

  return new;
}

void insertArrayDeleted(Array_indexdeleted *array, Indexdeletedbook *index, int mode){

  Indexdeletedbook **tmp;
  int pos;
  int first, last, middle;

  if (!array || !index || mode < BESTFIT || mode > FIRSTFIT)
  {
    return;
  }

  if (array->used == array->size)
  {
    array->size *= 2;
    tmp = realloc(array->indexdeleted_array, array->size * sizeof(Indexdeletedbook *));
    if(!tmp) return;
    array->indexdeleted_array = tmp;
  }

  if(mode == FIRSTFIT){
    pos = array->used;

  } else if (mode == BESTFIT || mode == WORSTFIT){
    if (array->used == 0){
      pos = 0;
    } else {
      first = 0;
      last = array->used - 1;
      while(first <= last){
        middle = (first + last)/2;
        if ((array->indexdeleted_array)[middle]->size == index->size){
          pos = middle + 1;
          break;
        } else if ((array->indexdeleted_array)[middle]->size > index->size){
          last = middle - 1;
        } else {
          first = middle + 1;
        }
      }
      pos = first;
    }
    memmove(&array->indexdeleted_array[pos + 1], &array->indexdeleted_array[pos], (array->used - pos)*sizeof(Indexdeletedbook *));

  } else {
    return;
  }

  array->indexdeleted_array[pos] = index;
  array->used++;

  return;
}

int deleteArrayDeleted(Array_indexdeleted *array, int bookId){ }

void freeArrayDeleted(Array_indexdeleted *array){
  size_t i;

  for (i = 0; i < array->used; i++)
  {
    free_Indexdeleted(array->indexdeleted_array[i]);
  }
  
  /*Free allocated memory for the array*/
  free(array->indexdeleted_array);
  array->indexdeleted_array = NULL;
  array->used = array->size = 0;

  return;
}

void printArrayDeleted(Array_indexdeleted *array){
  size_t i;

  printf("Size: %ld\n", array->size);
  printf("Used: %ld\n", array->used);
  printf("Indexdeleted_array: (offset, size)\n");
  for(i=0; i<array->used; i++){
    printf("(%ld, %ld)\n", array->indexdeleted_array[i]->offset, array->indexdeleted_array[i]->size);
  }
  printf("\n");

  return;
}

void findgapDeleted(Array_indexdeleted *array, size_t size, size_t *offset, int mode){
  size_t i;
  int pos = -1;
  size_t newsize;
  size_t bestSize;
  
  if (!array || size == 0 || !offset || mode < BESTFIT || mode > FIRSTFIT)
  {
    return;
  }

  *offset = EOF;
  if (array->used == 0){
    return;
  }

  bestSize = array->indexdeleted_array[0]->size;
  if(mode == BESTFIT){
    for (i = 0; i < array->size; i++) {
      newsize = array->indexdeleted_array[i]->size;

      if (newsize >= size + HEADER && newsize < bestSize) {
        bestSize = newsize;
        pos = i;
      }
    }

  } else if (mode == WORSTFIT){
    if(size + HEADER <= array->indexdeleted_array[array->used - 1]->size){
      pos = array->used - 1;
    }

  } else if (mode == FIRSTFIT){
    for (i = 0; i < array->size; i++) {
      newsize = array->indexdeleted_array[i]->size;
      
      if (newsize >= size + HEADER) {
        pos = i;
        break;
      }
    }

  } else {
    return;
  }

  if(pos < 0){
    printf("No se ha podido encontrar hueco\n");
    return;
  } else {
    *offset = array->indexdeleted_array[pos]->offset;
  }

  return;
}