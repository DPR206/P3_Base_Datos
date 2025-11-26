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

Indexbook *find_index_fromId(Array_index *array, int bookId, int beginning, int end, int *pos){
  int middle;

  if (!array || end < beginning || bookId < 0)
  {
    return -1;
  }

  while (beginning < end)
  {
    middle = (beginning-end)/2;

    if (array->index_array[middle].key == bookId)
    {
      *pos = middle;
      return array->index_array[middle];
    } 
    else if (array->index_array[middle].key > bookId)
    {
      end = middle -1;
    } 
    else if (array->index_array[middle].key < bookId)
    {
      beginning = middle+1;
    } 
  }
  
  return NULL;
}

int bin_search_find(Indexbook **array, Indexbook *index_search, int beginning, int end)
{
  int pos;
  int middle;

  if (!array || end < beginning || sizeof(array) == 0 || !index_search || !*array)
  {
    return ERROR;
  }

  while (beginning < end)
  {
    middle = (beginning - end) / 2;

    if (array[middle]->key == index_search->key)
    {
      return middle;
    }
    else if (array[middle]->key > index_search->key)
    {
      end = middle - 1;
    }
    else if (array[middle]->key < index_search->key)
    {
      beginning = middle + 1;
    }
  }

  return ERROR;
}

int bin_search_delete(Indexbook **array, Indexbook *index_search, int beginning, int end)
{
  int pos;
  int middle;
  int end_aux = end;

  if (!array || !*array || !index_search || beginning>end || sizeof(array) == 0 )
  {
    return ERROR;
  }
  
  while (beginning < end)
  {
    middle = (beginning - end) / 2;

    if (array[middle-1]->key < index_search->key && array[middle]->key > index_search->key)
    {
      return middle;
    } 
    else if (array[middle]->key < index_search && array[middle+1]->key > index_search->key)
    {
      return middle++;
    }
    else if (array[middle]->key > index_search->key && array[middle-1]->key > index_search->key)
    {
      end = middle - 1;
    }
    else if (array[middle]->key < index_search->key && array[middle+1]->key < index_search)
    {
      beginning = middle + 1;
    }
  }

  if (array[end_aux]->key < index_search->key)
  {
    return end_aux++;
  }
  
  return ERROR;
}

void free_Indexbook(Indexbook *indexbook)
{
  free(indexbook);
  return;
}

void initArray(Array_index *ai, size_t initalSize)
{
  int i;

  if (!ai || initalSize < 0)
  {
    return;
  }

  /*Create initial empty array of size initialSize*/
  ai->index_array = (Indexbook **)calloc(initalSize, sizeof(Indexbook *));
  if (!ai->index_array)
  {
    return;
  }

  for (i = 0; i < initalSize; i++)
  {
    ai->index_array[i] = (Indexbook *)calloc(1, sizeof(Indexbook *));

    if (!ai->index_array[i])
    {
      freeArray(ai);
      return;
    }
  }

  ai->used = 0;
  ai->size = initalSize;

  return;
}

Status insertArray(Array_index *ai, Indexbook *index)
{
  int i;
  int pos;

  if (!ai || index->key < 0)
  {
    return ERROR;
  }

  for (i = 0; i < ai->used; i++)
  {
    if (ai->index_array[i]->key == index->key)
    {
      return ERROR; /*There cannot be duplicates*/
    }
  }

  /*Insert item index in array*/

  if (ai->used == ai->size)
  {
    /*ai->size is the number of entries*/
    ai->size *= 2;
    ai->index_array = realloc(ai->index_array, ai->size * sizeof(Indexbook));
  }

  pos = bin_search_delete(ai, index, 0, ai->used);

  if (pos == ERROR)
  {
    return ERROR;
  }

  memmove(ai->index_array+pos, ai->index_array+pos, ai->used-pos);
  ai->index_array[pos] = index;
  ai->used++;

  /*ai->used is the number of useds entries*/
  /*ai->index_array[ai->used++] = index;*/

  return OK;
}

Status deletetArray(Array_index *ai, Indexbook *index)
{
  int pos;

  if (!ai || index->key < 0)
  {
    return ERROR;
  }

  pos = bin_search_find(ai->index_array, index, 0, ai->used);
  if (pos == -1)
  {
    return ERROR;
  }

  free_Indexbook(&ai->index_array[pos]);

  memmove(ai->index_array + pos, ai->index_array + pos + 1, ai->used - pos);
  ai->used--;

  return OK;
}


void freeArray(Array_index *ai)
{
  int i;

  for (i = 0; i < ai->size; i++)
  {
    free_Indexbook(ai->index_array[i]);
  }

  /*Free allocated memory for the array*/
  free(ai->index_array);
  ai->index_array = NULL;
  ai->used = ai->size = 0;

  return;
}

void add(Array_index *indexarray, char *index_file, int bookId, char *info){
  int *pos = NULL;

  if(!indexarray || !index_file || bookId < 0 || !info){
    return;
  }

  index = find_index_fromId(indexarray, bookId, 0, indexarray->used, pos);
  if(index != NULL){
    fprintf(stdout, "Record with bookId=%d exists.\n", bookId);
    return;
  }

}

void find(Array_index *indexarray, char *index_file, int bookId){
  Indexbook *index=NULL;
  int *pos=NULL;
  FILE *flibrary = NULL;
  char *info = NULL;

  if(!indexarray || !index_register || bookId<0){
    return;
  }

  index = find_index_fromId(indexarray, bookId, 0, indexarray->used, pos);
  if(!index || !pos){
    fprintf(stdout, "Record with bookId=%d does not exist\n", bookId);
    return;
  }

  flibrary = fopen(index_file, "rb+");
  fseek(flibrary, pos);
  if (fread(info, index->size, 1, flibrary) < 1){
    return;
  }

  fprintf(stdout, "%s\n", info);

  fclose(flibrary);
  free_Indexbook(index);
  return;
}

/* por terminar */
void del(Array_index *indexarray, Array_indexdeleted *indexdeletedarray, char *indexdeleted_file, int bookId){
  Indexbook *index = NULL;
  int *pos=NULL;

  if(!indexarray || !indexdeletedarray || !index_register || !indexdeleted_register || bookId<0){
    return;
  }

  index = find_index_fromId(indexarray, bookId, 0, indexarray->used, pos);
  if(!index || !pos){
    fprintf(stdout, "Record with bookId=%d does not exist\n", bookId);
    return;
  }

  /* Se añade una entrada en el array de borrados*/
  /* FALTA ESCRIBIR EN EL REGISTRO */
  insertArray(indexdeletedarray, index);

  /* Se elimina del array de índices */
  deleteArray(indexarray, index);

  fprintf(stdout, "Record with bookId=%d has been deleted\n", bookId);
  return;
}

Status save_index(char *filename, Array_index *ai)
{
  FILE *index_file = NULL;
  int i;
  /*int error;*/ /*Habría que hacer un control de errores de fwrite*/

  if (!filename || !ai || !ai->index_array)
  {
    return ERROR;
  }
  
  index_file = fopen(filename, "wb");

  if (!index_file)
  {
    return ERROR;
  }
  
  for ( i = 0; i < ai->used; i++)
  {
    fwrite(&ai->index_array[i]->key, sizeof(int), 1, index_file);
    fwrite(&ai->index_array[i]->offset, sizeof(long int), 1, index_file);
    fwrite(&ai->index_array[i]->size, sizeof(size_t), 1, index_file);
  }
  
  fclose(index_file);
  index_file = NULL;

  return OK;
}

Array_index *load_index(char *filename)
{
  Array_index *new_array = NULL;
  FILE *index_file = NULL;
  Indexbook *new_index = NULL;
  Status st = OK;
  int *key_aux;
  long int *offset_aux;
  size_t *size_aux;

  if (!filename)
  {
    return NULL;
  }
  
  new_array = (Array_index *)calloc(1, sizeof(Array_index));

  if (!new_array)
  {
    return NULL;
  }
  
  index_file = fopen(filename, "rb");

  if (!index_file)
  {
    return NULL;
  }

  initArray(new_array, 5);

  while (!feof(index_file))
  {
    fread(key_aux, sizeof(int), 1, index_file);
    fread(offset_aux, sizeof(long int), 1, index_file);
    fread(size_aux, sizeof(size_t), 1, index_file);

    new_index = create_Indexbook(key_aux, offset_aux, size_aux);

    if (!new_index)
    {
      return NULL;
    }
    
    st = insertArray(new_array, new_index);

    if (st == ERROR)
    {
      return ERROR;
    }
    
  }
  
  return new_array;
}
