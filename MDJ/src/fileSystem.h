#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <readline/readline.h>
#include <dirent.h>
#include <unistd.h>
#include <commons/collections/list.h>
#include "operacionesArchivos.h"
#include "parser.h"


int crearArchivoFS(char* rutaArchivo, char* datosTotales);
int borrarArchivoFS(char* rutaArchivo);

extern int TAMANIO_BLOQUES;
extern t_log* logger;

#endif /*FILESYSTEM_H_*/
