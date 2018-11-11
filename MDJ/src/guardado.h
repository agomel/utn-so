#ifndef GUARDADO_H_
#define GUARDADO_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <readline/readline.h>
#include <dirent.h>
#include <unistd.h>
#include "comandos.h"
#include "operaciones.h"
#include <commons/collections/list.h>


extern char* PUNTO_MONTAJE_ARCHIVOS;
extern char* PUNTO_MONTAJE_METADATA;
extern char* PUNTO_MONTAJE_BLOQUES;

char* rutaBitmap;
extern int TAMANIO_BLOQUES;
extern int CANTIDAD_BLOQUES;

extern t_log* logger;
void initGuardado();
void verificarExistenciaDeBitmap();

#endif /*GUARDADO_H_*/
