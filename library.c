#include <stdio.h>
#include <stdlib.h>

#include "library.h"

int main(int argc, char *argv[]) {
  Array_index *indexarray = NULL;
  Array_indexdeleted *deletedarray = NULL;

  int mode;
  char data_name[MAX_LEN];
  char index_name[MAX_LEN];
  char deleted_name[MAX_LEN];
  FILE *fdata = NULL, *fdeleted = NULL, *findex = NULL;

  char line[MAX_LEN];
  char *cmd = NULL;
  char *arg = NULL;
  Command code = UNKNOWN;
  int finished = 0;
  int bookId;

  /* Comprobar numero argumentos */
  if(argc != 3){
    printf("Missing argument\n");
    return OK;
  }

  /* Seleccionar mode */
  if (strcmp(argv[1], "best_fit") == 0) {
    mode = BESTFIT;
  }
  else if (strcmp(argv[1], "first_fit") == 0) {
    mode = FIRSTFIT;
  }
  else if (strcmp(argv[1], "worst_fit") == 0) {
    mode = WORSTFIT;
  }
  else {
    printf("Unknown search strategy unknown_search_strategy\n");
    return OK;
  }

  /* Seleccionar root */
  sprintf(data_name, "%s.db", argv[2]);
  sprintf(index_name, "%s.ind", argv[2]);
  sprintf(deleted_name, "%s.lst", argv[2]);

  /* Abrir ficheros de datos */
  fdata = fopen(data_name, "rb+");
  if(!fdata) fdata = fopen(data_name, "wb+");
  findex = fopen(index_name, "rb+");
  if(!findex) findex = fopen(index_name, "wb+");
  fdeleted = fopen(deleted_name, "rb+");
  if(!fdeleted) fdeleted = fopen(deleted_name, "wb+");
  if(!fdata || !findex || !fdeleted){
    printf("Error: couldn't open one of the files.\n");
    return ERR;
  }
  
  /* Inicializar arrays de indices y registros borrados */
  indexarray = initArray(INIT_SIZE);
  if(!indexarray){
    fclose(fdata);
    fclose(findex);
    fclose(fdeleted);
    return ERR;
  }
  deletedarray = initArrayDeleted(INIT_SIZE);
  if(!deletedarray){
    fclose(fdata);
    fclose(findex);
    fclose(fdeleted);
    freeArray(indexarray);
    return ERR;
  }

  if(comand_load(indexarray, deletedarray, findex, fdeleted, mode) == ERR){
    comand_exit(indexarray, deletedarray, fdata, findex, fdeleted, mode);
    return ERR;
  }
  fprintf(stdout, "Type command and argument/s.\n");
  fprintf(stdout, "exit\n");

  /* Loop */
  while (fgets(line, sizeof(line), stdin) != NULL){
    line[strcspn(line, "\r\n")] = '\0';
    if (line[0] == '\0') continue;

    /* Recopilar comando y argumento */
    cmd = strtok(line, " \t");
    if (!cmd) continue;
    arg = line + strlen(cmd) + 1;
    printf("\n");

    /* Procesar comando */
    if (strcmp(cmd, "add") == 0) {
      code = ADD;
    } else if (strcmp(cmd, "find") == 0) {
      code = FIND;
    } else if (strcmp(cmd, "del") == 0) {
      code = DEL;
    } else if (strcmp(cmd, "exit") == 0) {
      code = EXIT;
    } else if (strcmp(cmd, "printInd") == 0) {
      code = PRINTIND;
    } else if (strcmp(cmd, "printLst") == 0) {
      code = PRINTLST;
    } else if (strcmp(cmd, "printRec") == 0) {
      code = PRINTREC;
    } else {
      code = UNKNOWN;
    }

    /* Ejecutar comando */
    switch (code) {
      case ADD:
        if(!arg){
          printf("Introduce book information to add.\n");
          break;
        }
        if(comand_add(indexarray, deletedarray, fdata, arg, mode) == ERR){
          comand_exit(indexarray, deletedarray, fdata, findex, fdeleted, mode);
          finished = 1;
        }
        break;
      case FIND:
        if(!arg){
            printf("Introduce bookId to find.\n");
            break;
        }
        bookId = atoi(arg);
        if(comand_find(indexarray, fdata, bookId) == ERR){
          comand_exit(indexarray, deletedarray, fdata, findex, fdeleted, mode);
          finished = 1;
        }
        break;
      case DEL:
        if(!arg){
            printf("Introduce bookId to delete.\n");
            break;
        }
        bookId = atoi(arg);
        if(comand_del(indexarray, deletedarray, bookId, mode) == ERR){
          comand_exit(indexarray, deletedarray, fdata, findex, fdeleted, mode);
          finished = 1;
        }
        break;
      case EXIT:
        comand_exit(indexarray, deletedarray, fdata, findex, fdeleted, mode);
        finished = 1;
        break;
      case PRINTIND:
        if(comand_printInd(indexarray) == ERR){
          comand_exit(indexarray, deletedarray, fdata, findex, fdeleted, mode);
          finished = 1;
        }
        break;
      case PRINTLST:
        if(comand_printLst(deletedarray) == ERR){
          comand_exit(indexarray, deletedarray, fdata, findex, fdeleted, mode);
          finished = 1;
        }
        break;
      case PRINTREC:
        if(comand_printRec(indexarray, fdata) == ERR){
          comand_exit(indexarray, deletedarray, fdata, findex, fdeleted, mode);
          finished = 1;
        }
        break;
      case UNKNOWN:
        break;
    }

    if(finished == 1){
      break;
    } else {
        fprintf(stdout, "exit\n");
    }
  }

  return OK;
}