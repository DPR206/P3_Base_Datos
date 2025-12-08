#include "index.h"


/********************************/
/*      FUNCIONES INDEXBOOK     */
/********************************/



Indexbook *create_Indexbook(int key, long int offset, size_t size)
{
  Indexbook *new_index = NULL;

  if (key < 0 || offset < 0 || size == 0)
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

  /* Se inserta ordenado utilizando bbin */
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
  long unsigned i;

  printf("%ld\n", ai->used);
  for (i = 0; i < ai->used; i++)
  {
    if(ai->index_array[i]){
      free_Indexbook(ai->index_array[i]);
    }
  }
  
  /*Free allocated memory for the array*/
  free(ai->index_array);
  free(ai);

  return;
}

void printArray(Array_index *array){
  long unsigned i;

  printf("Size: %ld\n", array->size);
  printf("Used: %ld\n", array->used);
  printf("Index_array: (key, offset, size)\n");
  for (i=0; i < array->used; i++){
    printf("(%d, %ld, %ld)\n", array->index_array[i]->key, array->index_array[i]->offset, array->index_array[i]->size);
  }
  printf("\n");

  return;
}