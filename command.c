#include <stdio.h>
#include "command.h"

Status findgapDeleted(Array_index *indexarray, Array_indexdeleted *array, size_t size, long int *offset, int mode){
  size_t i;
  int pos = -1;
  size_t bestsize;
  size_t newsize, newoffset;
  Indexdeletedbook *newdeleted = NULL;
  
  if (!indexarray || !indexarray->index_array || !array || !array->indexdeleted_array) return ERR;
  if (size == 0 || !offset) return ERR;
  if(mode < BESTFIT || mode > FIRSTFIT) return ERR;

  /* Si no existen huecos (registros borrados)*/
  if (array->used == 0){
    *offset = EOF;
    return OK;
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
    if(size <= array->indexdeleted_array[0]->size){
      pos = 0;
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
    return ERR;
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
      memmove(&array->indexdeleted_array[pos], &array->indexdeleted_array[pos + 1], (array->used - pos - 1) * sizeof(Indexdeletedbook *));
      array->used--;
    }

    /* Insertar nuevo hueco que sobra */
    if(newsize > HEADER) {
      newdeleted = create_Indexdeleted(newoffset, newsize - HEADER);
      if(!newdeleted){
        return ERR;
      }
      if(insertArrayDeleted(array, newdeleted, mode) == ERR){
        free_Indexdeleted(newdeleted);
        return ERR;
      }
    }
  }

  return OK;
}

Status comand_load(Array_index *indexarray, Array_indexdeleted *deletedarray, FILE *findex, FILE *fdeleted, int mode){
  int strategy;
  int key;
  size_t offset, size;

  Indexbook *index = NULL;
  Indexdeletedbook * deleted;

  if(!indexarray || !indexarray->index_array || !deletedarray || !deletedarray->indexdeleted_array) return ERR;
  if(!findex || !fdeleted) return ERR;
  if(mode < BESTFIT || mode > FIRSTFIT) return ERR;

  fseek(findex, 0, SEEK_SET);
  fseek(fdeleted, 0, SEEK_SET);

  while (1){
    if(fread(&key, S_INT, 1, findex) != 1) break;
    if(fread(&offset, S_SIZET, 1, findex) != 1) break;
    if(fread(&size, S_SIZET, 1, findex) != 1) break;

    index = create_Indexbook(key, offset, size);
    if(!index) return ERR;

    if(insertArray(indexarray, index) == ERR){
      free_Indexbook(index);
      return ERR;
    }
  }
  fclose(findex);

  if (fread(&strategy, S_INT, 1, fdeleted) != 1) {
    fclose(fdeleted);
    return OK;
  }
  while (1) {

    if(fread(&offset, S_SIZET, 1, fdeleted) != 1) break;
    if(fread(&size, S_SIZET, 1, fdeleted) != 1) break;

    deleted = create_Indexdeleted(offset, size);
    if(!deleted) return ERR;

    if(strategy == mode){
      if(insertDeletedMode(deletedarray, deleted) == ERR){
        free_Indexdeleted(deleted);
      return ERR;
      }
    } else{
      if(insertArrayDeleted(deletedarray, deleted, mode) == ERR){
        free_Indexdeleted(deleted);
        return ERR;
      }
    }
  }
  fclose(fdeleted);

  return OK;
}



/********************************/
/*     COMANDOS PRINCIPALES     */
/********************************/



Status comand_add(Array_index *indexarray, Array_indexdeleted *deletedarray, FILE *fdata, char *info, int mode){
  Indexbook *index = NULL;
  int pos;

  int key;
  char isbn[ISBN + 1];
  char title[MAX_LEN];
  char editorial[MAX_LEN];
  char barra = '|';
  char *toks;
  size_t infosize;
  size_t titlesize;
  size_t editorialsize;

  size_t size;
  long int offset;
  size_t printed_size = 0;

  if(!indexarray || !indexarray->index_array || !deletedarray || !deletedarray->indexdeleted_array) return ERR;
  if(!fdata || !info) return ERR;
  if(mode < BESTFIT || mode > FIRSTFIT) return ERR;

  /* Comprobar la cadena que se manda es correcta */
  infosize = strlen(info);
  if(infosize == 0){
    return ERR;
  }
  /* Cargar los campos del libro */
  toks = strtok(info, "|");
  key = atoi(toks);

  toks = strtok(NULL, "|");
  if(toks != NULL){
    strcpy(isbn, toks);
  } else {
    return ERR;
  }

  toks = strtok(NULL, "|");
  if(toks != NULL){
    strcpy(title, toks);
  } else {
    return ERR;
  }

  toks = strtok(NULL, "|");
  if(toks != NULL){
    strcpy(editorial, toks);
  } else {
    return ERR;
  }

  /* Comprobar que el libro no existe */
  index = find_index_fromId(indexarray, key, 0, indexarray->used - 1, &pos);
  if(index != NULL){
    fprintf(stdout, "Record with bookId=%d exists\n", key);
    return OK;
  }

  titlesize = strlen(title);
  editorialsize = strlen(editorial);
  size = S_INT + ISBN + titlesize + 1 + editorialsize;

  /* Encontrar donde insertarlo en el fichero */
  if(findgapDeleted(indexarray, deletedarray, size, &offset, mode) == ERR) return ERR;

  /* Ir a la posicion donde escribir */
  if(offset == EOF){
    fseek(fdata, 0, SEEK_END);
    offset = ftell(fdata);
  } else {
    fseek(fdata, offset, SEEK_SET);
  }

  /* Escribir en archivo */
  printed_size += fwrite(&size, HEADER, 1, fdata);
  printed_size += fwrite(&key, S_INT, 1, fdata);
  printed_size += fwrite(isbn, 1, ISBN, fdata);
  printed_size += fwrite(title, 1, titlesize, fdata);
  printed_size += fwrite(&barra, 1, 1, fdata);
  printed_size += fwrite(editorial, 1, editorialsize, fdata);

  if(printed_size == 0){
    return ERR;
  }

  /* Crear nuevo indice e insertar en el array */
  index = create_Indexbook(key, offset, size);
  if(!index) return ERR;

  if(insertArray(indexarray, index) == ERR){
    free_Indexbook(index);
    return ERR;
  }

  fprintf(stdout, "Record with BookID=%d has been added to the database\n", key);

  return OK;
}

Status comand_find(Array_index *indexarray, FILE *fdata, int bookId){
  Indexbook *index=NULL;
  int pos=0;
  int key;
  size_t bookSize;
  char isbn[ISBN + 1], book[MAX_LEN]; 

  if(!indexarray || !indexarray->index_array || !fdata || bookId<0) return ERR;

  /* Busca el indice si existe */
  index = find_index_fromId(indexarray, bookId, 0, indexarray->used - 1, &pos);
  if(!index){
    fprintf(stdout, "Record with bookId=%d does not exist\n\n", bookId);
    return OK;
  }

  /* Leer la informacion desde la posicion */
  fseek(fdata, index->offset + HEADER, SEEK_SET);
  if (fread(&key, S_INT, 1, fdata) != 1) return ERR;
  if (fread(isbn, ISBN, 1, fdata) != 1) return ERR;
  isbn[ISBN] = '\0';
  bookSize = index->size - S_INT - ISBN;
  if (fread(book, bookSize, 1, fdata) != 1) return ERR;
  book[bookSize] = '\0';

  /* Imprimir mensaje */
  fprintf(stdout, "%d|%s|%s\n", key, isbn, book);

  return OK;
}

Status comand_del(Array_index *indexarray, Array_indexdeleted *indexdeletedarray, int bookId, int mode){
  Indexdeletedbook *deleted = NULL;
  Indexbook *index_delete = NULL;

  if(!indexarray || !indexarray->index_array || !indexdeletedarray || !indexdeletedarray->indexdeleted_array || bookId<0) return ERR;
  if(mode < BESTFIT || mode > FIRSTFIT) return ERR;

  /* Se elimina de los índices */
  if (deleteArray(indexarray, bookId, &index_delete) == ERR){
    fprintf(stdout, "Item with key %d does not exist\n", bookId);
    return OK;
  }
  
  /* Se crea un indexdeleted con el hueco que deja este */
  deleted = create_Indexdeleted(index_delete->offset, index_delete->size);
  if (!deleted){
    return ERR;
  }
  if(insertArrayDeleted(indexdeletedarray, deleted, mode) == ERR){
    free_Indexdeleted(deleted);
    return ERR;
  }

  /* Se libera memoria del indice eliminado */
  free_Indexbook(index_delete);
  fprintf(stdout, "Record with BookID=%d has been deleted\n", bookId);

  return OK;
}

void comand_exit(Array_index *indexarray, Array_indexdeleted *indexdeletedarray, FILE *fdata, FILE *findex, FILE *fdeleted, int mode){
  size_t i;
  size_t printed_size = 0;

  if(!indexarray || !indexdeletedarray || !indexarray->index_array || !indexdeletedarray->indexdeleted_array) return;
  if(!fdata || !findex || !fdeleted) return;
  if(mode < BESTFIT || mode > FIRSTFIT) return;

  /* Cerrar el archivo de datos */
  fclose(fdata);
  
  fseek(findex, 0, SEEK_SET);
  fseek(fdeleted, 0, SEEK_SET);

  /* Guardar indices */
  for(i=0; i<indexarray->used; i++){
    
    /* Escribir en archivo */
    printed_size += fwrite(&indexarray->index_array[i]->key, S_INT, 1, findex);
    printed_size += fwrite(&indexarray->index_array[i]->offset, S_SIZET, 1, findex);
    printed_size += fwrite(&indexarray->index_array[i]->size, S_SIZET, 1, findex);

    if (printed_size == 0) return;
  }
  freeArray(indexarray);
  fclose(findex);

  /* Guardar deleted */
  printed_size += fwrite(&mode, S_INT, 1, fdeleted);
  for(i=0; i<indexdeletedarray->used; i++){

    /* Escribir en archivo */
    printed_size += fwrite(&indexdeletedarray->indexdeleted_array[i]->offset, S_SIZET, 1, fdeleted);
    printed_size += fwrite(&indexdeletedarray->indexdeleted_array[i]->size, S_SIZET, 1, fdeleted);

    if (printed_size == 0) return;
  }
  freeArrayDeleted(indexdeletedarray);
  fclose(fdeleted);

  return;
}

Status comand_printInd(Array_index *indexarray){
  long unsigned i;
  if(!indexarray || !indexarray->index_array) return ERR;

  for (i=0; i < indexarray->used; i++){
    fprintf(stdout, "Entry #%ld\n", i);
    fprintf(stdout, "    key: #%d\n    offset: #%ld\n    size: #%ld\n", indexarray->index_array[i]->key, indexarray->index_array[i]->offset, indexarray->index_array[i]->size);
  }
  return OK;
}

Status comand_printLst(Array_indexdeleted *deletedarray){
  long unsigned i;
  if(!deletedarray || !deletedarray->indexdeleted_array) return ERR;

  for (i=0; i < deletedarray->used; i++){
    fprintf(stdout, "Entry #%ld\n", i);
    fprintf(stdout, "    offset: #%ld\n    size: #%ld\n", deletedarray->indexdeleted_array[i]->offset, deletedarray->indexdeleted_array[i]->size);
  }
  return OK;
}

Status comand_printRec(Array_index *indexarray, FILE *fdata){
  long unsigned i;
  int key;
  size_t bookSize;
  char isbn[ISBN + 1], book[MAX_LEN]; 

  if(!indexarray || !indexarray->index_array || !fdata) return ERR;

  for (i=0; i < indexarray->used; i++){
    /* Leer la informacion desde la posicion */
    fseek(fdata, indexarray->index_array[i]->offset + HEADER, SEEK_SET);
    if (fread(&key, S_INT, 1, fdata) != 1) return ERR;
    if (fread(isbn, ISBN, 1, fdata) != 1) return ERR;
    isbn[ISBN] = '\0';
    bookSize = indexarray->index_array[i]->size - S_INT - ISBN;
    if (fread(book, bookSize, 1, fdata) != 1) return ERR;
    book[bookSize] = '\0';

    /* Imprimir mensaje */
    fprintf(stdout, "%d|%s|%s\n", key, isbn, book);
  }
  return OK;
}