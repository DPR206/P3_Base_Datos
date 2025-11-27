#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "index.h"

Indexbook *create_Indexbook(int key, long int offset, size_t size)
{
  Indexbook *new_index = NULL;

  if (key < 0 || offset < 0 || size < 0)
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

Indexbook *find_index_fromId(Array_index *array, int bookId, int beginning, int end, int *pos){
  int middle;

  if (!array || end < beginning || bookId < 0)
  {
    return NULL;
  }

  while (beginning <= end)
  {
    middle = (beginning + end)/2;

    if (array->index_array[middle]->key == bookId)
    {
      *pos = middle;
      return array->index_array[middle];
    } 
    else if (array->index_array[middle]->key > bookId)
    {
      end = middle -1;
    } 
    else if (array->index_array[middle]->key < bookId)
    {
      beginning = middle+1;
    } 
  }
  
  return NULL;
}

/* */
int bin_search_find(Array_index *array, Indexbook *index_search, int beginning, int end)
{
  int pos;
  int middle;

  if (!array || end < beginning || sizeof(array) == 0)
  {
    return -1;
  }

  while (beginning <  end)
  {
    middle = (beginning+end)/2;

    if (array->index_array[middle]->key == index_search->key)
    {
      return middle;
    } 
    else if (array->index_array[middle]->key > index_search->key)
    {
      end = middle -1;
    } 
    else if (array->index_array[middle]->key < index_search->key)
    {
      beginning = middle+1;
    }
    
    
  }
  
  return -1;
}

/* */
int bin_search_delete(Array_index *array, Indexbook *index_search, int beginning, int end)
{
  int pos;
  int middle;
  int end_aux = end;

  if (!array || !index_search || beginning>end || sizeof(array) == 0 )
  {
    return -1;
  }
  
  while (beginning < end)
  {
    middle = (beginning + end) / 2;

    if (array->index_array[middle-1]->key < index_search->key && array->index_array[middle]->key > index_search->key)
    {
      return middle;
    } 
    else if (array->index_array[middle]->key < index_search && array->index_array[middle+1]->key > index_search->key)
    {
      return middle++;
    }
    else if (array->index_array[middle]->key > index_search->key && array->index_array[middle-1]->key > index_search->key)
    {
      end = middle - 1;
    }
    else if (array->index_array[middle]->key < index_search->key && array->index_array[middle+1]->key < index_search)
    {
      beginning = middle + 1;
    }
  }

  if (array->index_array[end_aux]->key < index_search->key)
  {
    return end_aux++;
  }
  
  return;
}

void free_Indexbook(Indexbook *indexbook)
{
  free(indexbook);
  return;
}

Array_index *initArray(size_t initialSize)
{
  Array_index *new = NULL;

  if (initialSize == 0)
  {
    return NULL;
  }
  
  new = (Array_index *)calloc(1, sizeof(Array_index));

  /*Create initial empty array of size initialSize*/
  new->index_array = (Indexbook **)calloc(initialSize, sizeof(Indexbook *));
  if (!new->index_array)
  {
    free(new);
    return NULL;
  }
  
  new->used = 0;
  new->size = initialSize;

  return new;
}

void insertArray(Array_index *ai, Indexbook *index)
{
  int i;
  int pos;
  int first, last, middle;
  Indexbook **tmp;

  if (!ai || !index)
  {
    return;
  }

  /* Expandir si es necesario */
  if (ai->used == ai->size)
  {
    ai->size *= 2;
    tmp = realloc(ai->index_array, ai->size * sizeof(Indexbook *));
    if(!tmp) return;
    ai->index_array = tmp;
  }

   /*Insert index in array*/
  if (ai->used == 0){
    pos = 0;
  } else {
    first = 0;
    last = ai->used - 1;
    while(first <= last){
      middle = (first + last)/2;
      if ((ai->index_array)[middle]->key == index->key){
        return; /* Duplicado */
      } else if ((ai->index_array)[middle]->key > index->key){
        last = middle - 1;
      } else {
        first = middle + 1;
      }
    }
    pos = first;
  }

  memmove(&ai->index_array[pos + 1], &ai->index_array[pos], (ai->used - pos)*sizeof(Indexbook *));
  ai->index_array[pos] = index;
  ai->used++;

  return;
}

void deleteArray(Array_index *ai, Indexbook *index)
{
  int pos;

  if (!ai || !index)
  {
    return;
  }
  
  pos = bin_search_find(ai, index, 0, ai->used);
  if (pos == -1)
  {
    return;
  }
      
  memmove(ai->index_array+pos, ai->index_array+pos+1, (ai->used-pos)*sizeof(Indexbook *));
    
  return;
}


void freeArray(Array_index *ai)
{
  int i;

  for (i = 0; i < ai->used; i++)
  {
    free_Indexbook(ai->index_array[i]);
  }
  
  /*Free allocated memory for the array*/
  free(ai->index_array);
  ai->index_array = NULL;
  ai->used = ai->size = 0;

  return;
}

/* COMANDOS PRINCIPALES*/
/*por terminar*/
void add(Array_index *indexarray, char *index_file, int key, long int offset, size_t size, char *info){
  Indexbook *index = NULL;
  int pos;

  if(!indexarray || !index_file || key < 0 || !info){
    return;
  }

  index = find_index_fromId(indexarray, key, 0, indexarray->used - 1, &pos);
  if(index != NULL){
    fprintf(stdout, "Record with bookId=%d exists.\n", key);
    return;
  }

  index = create_Indexbook(key, offset, size);
  if(!index){
    return;
  }
  insertArray(indexarray, index);
  
  /* Escoger hueco y meter en el fichero */
  return;
}

void find(Array_index *indexarray, char *index_file, int bookId){
  Indexbook *index=NULL;
  int pos=0;
  FILE *flibrary = NULL;
  char *info = NULL;

  if(!indexarray || !index_file || bookId<0){
    return;
  }

  index = find_index_fromId(indexarray, bookId, 0, indexarray->used, &pos);
  if(!index){
    fprintf(stdout, "Record with bookId=%d does not exist\n", bookId);
    return;
  }

  flibrary = fopen(index_file, "rb+");
  if(!flibrary){
    return;
  }

  info = malloc(index->size + 1); 
  if (!info) {
    fclose(flibrary);
    return;
  }

  fseek(flibrary, pos, SEEK_SET);
  if (fread(info, index->size, 1, flibrary) < 1){
    fclose(flibrary);
    free(info);
    return;
  }
  fclose(flibrary);

  info[index->size] = '\0';

  fprintf(stdout, "%s\n", info);

  free(info);
  free_Indexbook(index);
  return;
}

/* por terminar */
void del(Array_index *indexarray, Array_indexdeleted *indexdeletedarray, char *indexdeleted_file, int bookId){
  Indexbook *index = NULL;
  int *pos=NULL;

  if(!indexarray || !indexdeletedarray || !indexdeleted_file || bookId<0){
    return;
  }

  index = find_index_fromId(indexarray, bookId, 0, indexarray->used, pos);
  if(!index || !pos){
    fprintf(stdout, "Record with bookId=%d does not exist\n", bookId);
    return;
  }

  /* Se añade una entrada en el array de borrados*/
  /* FALTA ESCRIBIR EN EL REGISTRO */
  insertArray(indexdeletedarray, index); /*Cambiar para array deleted*/

  /* Se elimina del array de índices */
  deleteArray(indexarray, index);

  fprintf(stdout, "Record with bookId=%d has been deleted\n", bookId);
  return;
}