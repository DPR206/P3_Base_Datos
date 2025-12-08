#include "command.h"

void findgapDeleted(Array_index *indexarray, Array_indexdeleted *array, size_t size, long int *offset, int mode){
  size_t i;
  int pos = -1;
  size_t bestsize;
  size_t newsize, newoffset;
  Indexdeletedbook *newdeleted = NULL;
  
  if (!indexarray || !array || !offset || mode < BESTFIT || mode > FIRSTFIT)
  {
    return;
  }

  /* Si no existen huecos (registros borrados)*/
  if (array->used == 0){
    *offset = EOF;
    return;
  }

  /* Busca donde insertar entre los huecos de borrados */
  /* BESTFIT */
  if(mode == BESTFIT){
    for (i = 0; i < array->used; i++) {
      bestsize = array->indexdeleted_array[i]->size;

      if (bestsize >= size) {
        pos = i;
        break;
      }
    }

  /* WORSTFIT */
  } else if (mode == WORSTFIT){
    if(size <= array->indexdeleted_array[array->used - 1]->size){
      pos = array->used - 1;
    }

  /* FIRSTFIT */
  } else if (mode == FIRSTFIT){
    for (i = 0; i < array->used; i++) {
      bestsize = array->indexdeleted_array[i]->size;
      
      if (bestsize >= size) {
        pos = i;
        break;
      }
    }

  } else {
    return;
  }

  /* No ha encontrado un hueco donde insertar */
  if(pos < 0){
    *offset = EOF;

  /* Se va a insertar en un hueco */
  } else {
    *offset = array->indexdeleted_array[pos]->offset;

    newoffset = *offset + HEADER + size;
    newsize = array->indexdeleted_array[pos]->size - size;

    /* Eliminar el hueco usado */
    free_Indexdeleted(array->indexdeleted_array[pos]);
    if(array->used > 0 && (long unsigned int)pos < array->used){
      memmove(&array->indexdeleted_array[pos], &array->indexdeleted_array[pos + 1], (array->used - pos) * sizeof(Indexdeletedbook *));
      array->used--;
    }

    /* Insertar nuevo hueco que sobra */
    if(newsize > HEADER) {
      newdeleted = create_Indexdeleted(newoffset, newsize - HEADER);
      if(!newdeleted){
        return;
      }
      insertArrayDeleted(array, newdeleted, mode);
    }
  }

  return;
}



/********************************/
/*     COMANDOS PRINCIPALES     */
/********************************/



void comand_add(Array_index *indexarray, Array_indexdeleted *deletedarray, FILE *fdata, char *info, int mode){
  Indexbook *index = NULL;
  int pos;

  int key;
  char *tkey;
  size_t size;
  long int offset;
  size_t printed_size;
  char *toprint = NULL;

  if(!indexarray || !deletedarray || !fdata || !info) return;
  if(mode < 0 || mode > 2) return;

  /* Comprobar que no existe */
  size = strlen(info);
  tkey = strtok(info, "|");
  info = strtok(NULL, "|");
  key = atoi(tkey);
  index = find_index_fromId(indexarray, key, 0, indexarray->used - 1, &pos);
  if(index != NULL){
    fprintf(stdout, "Record with bookId=%d exists\n\n", key);
    return;
  }

  /* Encontrar donde insertarlo en el fichero */
  findgapDeleted(indexarray, deletedarray, size, &offset, mode);

  /* Ir a la posicion donde escribir */
  if(offset == EOF){
    fseek(fdata, 0, SEEK_END);
    offset = ftell(fdata);
  } else {
    fseek(fdata, offset, SEEK_SET);
  }

  /* Reservar memoria para lo que vamos a imprimir */
  /* tamaño = HEADER + size */
  toprint = malloc(size + HEADER + 1);
  if (!toprint) return;

  /* Construir lo que irá al archivo */
  printed_size = snprintf(toprint, size + HEADER + 1, "%*d%s|%s", HEADER, (int)size, tkey, info);
  if (printed_size < HEADER) {
    free(toprint);
    return;
  }

  /* Escribir en archivo */
  if (printed_size != fwrite(toprint, 1, printed_size, fdata)) {
    free(toprint);
    return;
  }

  free(toprint);

  /* Crear nuevo indice e insertar en el array */
  index = create_Indexbook(key, offset, size);
  if(!index){
    return;
  }
  insertArray(indexarray, index);

  return;
}

void comand_find(Array_index *indexarray, FILE *fdata, int bookId){
  Indexbook *index=NULL;
  int pos=0;
  char *info = NULL;

  if(!indexarray || !fdata || bookId<0){
    return;
  }

  /* Busca el indice si existe */
  index = find_index_fromId(indexarray, bookId, 0, indexarray->used - 1, &pos);
  if(!index){
    fprintf(stdout, "Record with bookId=%d does not exist\n\n", bookId);
    return;
  }

  /* Reservar memoria para cadena de informacion */
  info = malloc(index->size + 1); 
  if (!info) {
    return;
  }

  /* Leer la informacion desde la posicion */
  fseek(fdata, index->offset + HEADER, SEEK_SET);
  if (fread(info, index->size, 1, fdata) < 1){
    free(info);
    return;
  }
  info[index->size] = '\0';

  /* Imprimir mensaje */
  fprintf(stdout, "%s\n", info);

  free(info);
  return;
}

void comand_del(Array_index *indexarray, Array_indexdeleted *indexdeletedarray, int bookId, int mode){
  Indexdeletedbook *deleted = NULL;
  Indexbook *index_delete = NULL;

  if(!indexarray || !indexdeletedarray || bookId<0) return;
  if(mode < 0 || mode > 2) return;

  /* Se elimina de los índices */
  if (deleteArray(indexarray, bookId, &index_delete) == -1){
    fprintf(stdout, "Record with bookId=%d does not exist\n\n", bookId);
    return;
  }
  
  /* Se crea un indexdeleted con el hueco que deja este */
  deleted = create_Indexdeleted(index_delete->offset, index_delete->size);
  if (!deleted){
    return;
  }
  insertArrayDeleted(indexdeletedarray, deleted, mode);

  /* Se libera memoria del indice eliminado */
  free_Indexbook(index_delete);
  fprintf(stdout, "Record with bookId=%d has been deleted\n\n", bookId);

  return;
}

void comand_exit(Array_index *indexarray, Array_indexdeleted *indexdeletedarray, FILE *fdata, FILE *findex, FILE *fdeleted){
  size_t i;
  char *toprint = NULL;
  size_t printed_size;

  /* Cerrar el archivo de datos */
  fclose(fdata);

  /* Guardar indices */
  toprint = malloc(INDEXDATA_SIZE + 1);
  if (!toprint) return;
  for(i=0; i<indexarray->used; i++){

    /* Construir lo que se escribe en el archivo */
    printed_size = snprintf(toprint, INDEXDATA_SIZE + 1, "%*d%*ld%*ld", S_INT, indexarray->index_array[i]->key, S_SIZET, indexarray->index_array[i]->offset, S_SIZET, indexarray->index_array[i]->size);
    if (printed_size != INDEXDATA_SIZE) {
      free(toprint);
      return;
    }

    /* Escribir en archivo */
    if (printed_size != fwrite(toprint, 1, printed_size, findex)) {
      free(toprint);
      return;
    }
  }
  freeArray(indexarray);
  fclose(findex);

  /* Guardar deleted */
  for(i=0; i<indexdeletedarray->used; i++){

    /* Construir lo que se escribe en el archivo */
    printed_size = snprintf(toprint, DELETEDDATA_SIZE + 1, "%*ld%*ld", S_SIZET, indexdeletedarray->indexdeleted_array[i]->offset, S_SIZET, indexdeletedarray->indexdeleted_array[i]->size);
    if (printed_size != DELETEDDATA_SIZE) {
      free(toprint);
      return;
    }

    /* Escribir en archivo */
    if (printed_size != fwrite(toprint, 1, printed_size, fdeleted)) {
      free(toprint);
      return;
    }
  }
  freeArrayDeleted(indexdeletedarray);
  fclose(fdeleted);

  free(toprint);
  return;
}

void comand_printInd(Array_index *indexarray){
  long unsigned i;
  if(!indexarray || !indexarray->index_array) return;

  for (i=0; i < indexarray->used; i++){
    fprintf(stdout, "Entry: #%ld\n", i);
    fprintf(stdout, "    key: #%d\n    offset: #%ld\n    size: #%ld\n", indexarray->index_array[i]->key, indexarray->index_array[i]->offset, indexarray->index_array[i]->size);
  }
  return;
}

void comand_printLst(Array_indexdeleted *deletedarray){
  long unsigned i;
  if(!deletedarray || !deletedarray->indexdeleted_array) return;

  for (i=0; i < deletedarray->used; i++){
    fprintf(stdout, "Entry: #%ld\n", i);
    fprintf(stdout, "    offset: #%ld\n    size: #%ld\n", deletedarray->indexdeleted_array[i]->offset, deletedarray->indexdeleted_array[i]->size);
  }
  return;
}

void comand_printRec(Array_index *indexarray, FILE *fdata){
  long unsigned i;
  char *info;
  if(!indexarray || !indexarray->index_array || !fdata) return;

  info = (char *)malloc(MAX_LEN);
  if (!info) {
    return;
  }

  for (i=0; i < indexarray->used; i++){
    /* Leer la informacion desde la posicion */
    fseek(fdata, indexarray->index_array[i]->offset + HEADER, SEEK_SET);
    fread(info, indexarray->index_array[i]->size, 1, fdata);
    info[indexarray->index_array[i]->size ] = '\0';

    /* Imprimir mensaje */
    fprintf(stdout, "  %s\n", info);
  }

  free(info);
  return;
}