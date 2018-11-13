#ifndef BLOQUES_H_
#define BLOQUES_H_


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <readline/readline.h>
#include <dirent.h>
#include <unistd.h>
#include <commons/collections/list.h>
#include "operacionesArchivos.h"


char* getPathDeBloque(int bloque);
int obtenerCantidadBloques(int tamanio);
int obtenerBloque();
int eliminarBloque(int bloque);
int guardarDatosEnBloque(char* rutaBloque, int tamanioAEscribir, int tamanioEscrito, char* datosTotales, int* error);
t_list* crearArchivoEnBloques(char* datosTotales, int* error);


extern char* PUNTO_MONTAJE_ARCHIVOS;
extern char* PUNTO_MONTAJE_METADATA;
extern char* PUNTO_MONTAJE_BLOQUES;

extern int TAMANIO_BLOQUES;
extern int CANTIDAD_BLOQUES;

extern t_log* logger;

#endif /*BLOQUES_H_*/
