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
  if (indexdeleted) free(indexdeleted);
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

  /* Create initial empty array of size initialSize */
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

Status insertArrayDeleted(Array_indexdeleted *array, Indexdeletedbook *index, int mode){
  Indexdeletedbook **tmp;
  int pos;
  int first, last, middle;
  size_t msize;

  if (!array || !array->indexdeleted_array || !index || mode < BESTFIT || mode > FIRSTFIT)
  {
    return ERR;
  }

  if (array->used == array->size)
  {
    array->size *= 2;
    tmp = realloc(array->indexdeleted_array, array->size * sizeof(Indexdeletedbook *));
    if(!tmp) return ERR;
    array->indexdeleted_array = tmp;
  }

  if(mode == FIRSTFIT){
    pos = array->used;

  } else if (mode == WORSTFIT){
    if (array->used == 0) {
      pos = 0;
    } else {
      first = 0;
      last = array->used - 1;

      while (first <= last) {
        middle = (first + last) / 2;
        msize = array->indexdeleted_array[middle]->size;

        if (msize == index->size) {
          pos = middle + 1;
          break;
        } else if (msize < index->size) {
          last = middle - 1;
        } else {
          first = middle + 1;
        }
      }
      if (first > last) {
        pos = first;
      }
    }
  }else if (mode == BESTFIT){
    if (array->used == 0){
      pos = 0;
    } else {
      first = 0;
      last = array->used - 1;
      while(first <= last){
        middle = (first + last)/2;
        msize = array->indexdeleted_array[middle]->size;

        if (msize == index->size){
          pos = middle + 1;
          break;
        } else if (msize > index->size){
          last = middle - 1;
        } else {
          first = middle + 1;
        }
      }
      if (first > last) {
        pos = first;
      }
    }
  } else {
    return ERR;
  }

  memmove(&array->indexdeleted_array[pos + 1], &array->indexdeleted_array[pos], (array->used - pos)*sizeof(Indexdeletedbook *));
  array->indexdeleted_array[pos] = index;
  array->used++;

  return OK;
}

Status insertDeletedMode(Array_indexdeleted *array, Indexdeletedbook *index){
  Indexdeletedbook **tmp;
  int pos;

  if (!array || !array->indexdeleted_array || !index) return ERR;

  if (array->used == array->size)
  {
    array->size *= 2;
    tmp = realloc(array->indexdeleted_array, array->size * sizeof(Indexdeletedbook *));
    if(!tmp) return ERR;
    array->indexdeleted_array = tmp;
  }

  pos = array->used;
  memmove(&array->indexdeleted_array[pos + 1], &array->indexdeleted_array[pos], (array->used - pos)*sizeof(Indexdeletedbook *));
  array->indexdeleted_array[pos] = index;
  array->used++;

  return OK;
}

void freeArrayDeleted(Array_indexdeleted *array){
  size_t i;

  if(!array || !array->indexdeleted_array) return;

  for (i = 0; i < array->used; i++)
  {
    if(array->indexdeleted_array[i]){
      free_Indexdeleted(array->indexdeleted_array[i]);
    }
  }
  
  /* Free allocated memory for the array */
  free(array->indexdeleted_array);
  free(array);

  return;
}

Status printArrayDeleted(Array_indexdeleted *array){
  size_t i;

  if(!array || !array->indexdeleted_array) return ERR;

  printf("Size: %ld\n", array->size);
  printf("Used: %ld\n", array->used);
  printf("Indexdeleted_array: (offset, size)\n");
  for(i=0; i<array->used; i++){
    printf("(%ld, %ld)\n", array->indexdeleted_array[i]->offset, array->indexdeleted_array[i]->size);
  }
  printf("\n");

  return OK;
}