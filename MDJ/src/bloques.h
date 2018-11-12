#ifndef BLOQUES_H_
#define BLOQUES_H_


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <readline/readline.h>
#include <dirent.h>
#include <unistd.h>
#include "operaciones.h"
#include <commons/collections/list.h>


extern char* PUNTO_MONTAJE_ARCHIVOS;
extern char* PUNTO_MONTAJE_METADATA;
extern char* PUNTO_MONTAJE_BLOQUES;

extern int TAMANIO_BLOQUES;
extern int CANTIDAD_BLOQUES;

extern t_log* logger;

#endif /*BLOQUES_H_*/
