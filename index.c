#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "index.h"

Indexbook *create_Indexbook(int key, long int offset, size_t size)
{
  Indexbook *new_index = NULL;

  if (sizeof(key) != 16 || offset < 0 || size < 0)
  {
    return NULL;
  }
  
  new_index = (Indexbook *)calloc(1, sizeof(Indexbook));

  if (!new_index)
  {
    return NULL;
  }
  
  new_index->key = key;
  new_index->offset = offset;
  new_index->size = size;

  return new_index;
}

int search_register(Indexbook *array, Indexbook index_search, int beginning, int end)
{
  int pos;
  int middle;

  if (!array || end < beginning || sizeof(array) == 0)
  {
    return -1;
  }

  while (beginning <  end)
  {
    middle = (beginning-end)/2;

    if (array[middle].key == index_search.key)
    {
      return middle;
    } 
    else if (array[middle].key > index_search.key)
    {
      end = middle -1;
    } 
    else if (array[middle].key < index_search.key)
    {
      beginning = middle+1;
    }
    
    
  }
  
  return -1;

}

void free_Indexbook(Indexbook *indexbook)
{
  free(indexbook);
  return;
}

void initArray(Array_index *ai, size_t initalSize)
{
  if (!ai || initalSize<0)
  {
    return;
  }
  
  /*Create initial empty array of size initialSize*/
  ai->index_array = (Indexbook *)calloc(initalSize, sizeof(Indexbook));
  if (!ai->index_array)
  {
    return;
  }
  
  ai->used = 0;
  ai->size = initalSize;

  return;
}

void insertArray(Array_index *ai, Indexbook index)
{
  int i;

  if (!ai || index.key<0)
  {
    return;
  }
  
  for ( i = 0; i < ai->used; i++)
  {
    if (ai->index_array[i].key == index.key)
    {
      return; /*There cannot be duplicates*/
    }
    
  }

  /*Insert item index in array*/
  
  if (ai->used == ai->size)
  {
    /*ai->size is the number of entries*/
    ai->size *= 2;
    ai->index_array = realloc(ai->index_array, ai->size * sizeof(Indexbook));
  }
  
  /*ai->used is the number of useds entries*/
  ai->index_array[ai->used++] = index;

  return;
}

void deletetArray(Array_index *ai, Indexbook index)
{
  int pos;

  if (!ai || index.key<0)
  {
    return;
  }
  
  pos = search_register(ai->index_array, index, 0, ai->used);
  if (pos == -1)
  {
    return;
  }
   
  free_Indexbook(&ai->index_array[pos]);
      
  memmove(ai->index_array+pos, ai->index_array+pos+1, ai->used-pos);
    
  return;
}


void freeArray(Array_index *ai)
{
  int i;

  for (i = 0; i < ai->used; i++)
  {
    free_Indexbook(&ai->index_array[i]);
  }
  
  /*Free allocated memory for the array*/
  free(ai->index_array);
  ai->index_array = NULL;
  ai->used = ai->size = 0;

  return;
}