#ifndef OPERACIONES_H_
#define OPERACIONES_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <commons/config.h>
#include <biblioteca/logger.h>
#include <biblioteca/utilidades.h>

int validarArchivo(int emisor);
int crearArchivo(int emisor);
void guardarDatos(int emisor);
char* obtenerDatos(int emisor);
int eliminarArchivo(int emisor);

extern t_log* logger;

#endif /*OPERACIONES_H_*/
