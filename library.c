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

    /* Comprobar numero argumentos */
    if(argc != 3){
        printf("Error: Input must be %s <strategy> <root>.\n", argv[0]);
        return ERR;
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
        printf("Error: Invalid strategy. Use best_fit, first_fit o worst_fit.\n");
        return ERR;
    }

    /* Seleccionar root */
    snprintf(data_name,  sizeof(data_name),  "%s.db", argv[2]);
    snprintf(index_name, sizeof(index_name), "%s.ind", argv[2]);
    snprintf(deleted_name, sizeof(deleted_name), "%s.lst", argv[2]);

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

    /* Loop */
    while (fgets(line, sizeof(line), stdin) != NULL){
        line[strcspn(line, "\r\n")] = '\0';
        if (line[0] == '\0') continue;

        /* Recopilar comando y argumento */
        cmd = strtok(line, " \t");
        if (!cmd) continue;
        arg = strtok(NULL, " \t");

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
            /* COMPROBAR QUE EXISTE EL ARG CUANDO SE NECESITE */
            case ADD:
                comand_add(indexarray, deletedarray, fdata, arg, mode);
                break;
            case FIND:
                /* ARG ES UNA CADENA Y BOOK_ID UN NUMERO */
                comand_find(indexarray, fdata, arg);
                break;
            case DEL:
                /* ARG ES UNA CADENA Y BOOK_ID UN NUMERO */
                comand_del(indexarray, deletedarray, arg, mode);
                break;
            case EXIT:
                comand_exit(indexarray, deletedarray, fdata, findex, fdeleted);
                finished = 1;
                break;
            case PRINTIND:
                comand_printInd(indexarray);
                break;
            case PRINTLST:
                comand_printLst(deletedarray);
                break;
            case PRINTREC:
                comand_printRec(indexarray, fdata);
                break;
            case UNKNOWN:
                break;
        }

        if(finished == 1){
            break;
        }
    }

    return OK;
}