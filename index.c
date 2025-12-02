#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "index.h"
#include "deleted.h"


/********************************/
/*      FUNCIONES INDEXBOOK     */
/********************************/



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

void free_Indexbook(Indexbook *indexbook)
{
  free(indexbook);
  return;
}



/********************************/
/*  FUNCIONES BUSQUEDA BINARIA  */
/********************************/



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

int bin_search(Array_index *array, int F, int L, Indexbook *index)
{
  int f=F, l=L, m;
  /* Comprobacion de errores */
  if (!array || !array->index_array || L<F || !index){
    return -1;
  }

  /* Busqueda binaria */
  while(f<=l){
    m = (f + l )/2;
    if(array->index_array[m]->key==index->key){
      return m;
    } else if (array->index_array[m]->key > index->key) {
      l = m - 1;
    } else {
      f = m + 1;
    }
  }

  /* No se ha encontrado la clave dentro de la tabla */
  return -1;
}



/********************************/
/*     FUNCIONES ARRAY_INDEX    */
/********************************/



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

int deleteArray(Array_index *ai, int bookId, Indexbook **indexdeleted)
{
  int pos;
  Indexbook *index=NULL;

  if (!ai || bookId < 0)
  {
    return -1;
  }
  
  index = find_index_fromId(ai, bookId, 0, ai->used - 1, &pos);
  if (!index)
  {
    return -1;
  }

  *indexdeleted = index;
      
  memmove(ai->index_array+pos, ai->index_array+pos+1, (ai->used-pos)*sizeof(Indexbook *));
  ai->used--; 

  return 0;
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

void printArray(Array_index *array){
  int i;

  printf("Size: %ld\n", array->size);
  printf("Used: %ld\n", array->used);
  printf("Index_array: (key, offset, size)\n");
  for (i=0; i < array->used; i++){
    printf("(%d, %ld, %ld)\n", array->index_array[i]->key, array->index_array[i]->offset, array->index_array[i]->size);
  }
  printf("\n");

  return;
}



/********************************/
/*     COMANDOS PRINCIPALES     */
/********************************/



void add(Array_index *indexarray, Array_indexdeleted *deletedarray, char *index_file, int key, size_t size, char *info, int mode){
  Indexbook *index = NULL;
  int pos;
  size_t offset;

  if(!indexarray || !index_file || key < 0 || !info){
    return;
  }

  /* Comprobar que no existe ya */
  index = find_index_fromId(indexarray, key, 0, indexarray->used - 1, &pos);
  if(index != NULL){
    fprintf(stdout, "Record with bookId=%d exists\n\n", key);
    return;
  }

  /* Encontrar donde insertarlo en el fichero */
  findgapDeleted(deletedarray, size, &offset, mode);
  /* AQUI ESTA EL PROBLEMA */
  if(offset == -1){
    offset = 0;
  }
  printf("offset: %ld\n", offset);

  /* Crear nuevo indice y insertar en el array */
  index = create_Indexbook(key, offset, size);
  if(!index){
    return;
  }
  insertArray(indexarray, index);

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

  index = find_index_fromId(indexarray, bookId, 0, indexarray->used - 1, &pos);
  if(!index){
    fprintf(stdout, "Record with bookId=%d does not exist\n\n", bookId);
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
void del(Array_index *indexarray, Array_indexdeleted *indexdeletedarray, char *indexdeleted_file, int bookId, int mode){
  Indexdeletedbook *deleted = NULL;
  Indexbook *index_delete = NULL;

  if(!indexarray || !indexdeletedarray || !indexdeleted_file || bookId<0){
    return;
  }

  /* Se elimina de los índices */
  if (deleteArray(indexarray, bookId, &index_delete) == -1){
    fprintf(stdout, "Record with bookId=%d does not exist\n\n", bookId);
    return;
  }
  
  deleted = create_Indexdeleted(index_delete->offset, index_delete->size);
  if (!deleted){
    return;
  }

  free_Indexbook(index_delete);

  /* FALTA ESCRIBIR EN EL REGISTRO */
  insertArrayDeleted(indexdeletedarray, deleted, mode);

  fprintf(stdout, "Record with bookId=%d has been deleted\n\n", bookId);

  return;
}