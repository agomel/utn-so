#ifndef OPERACIONES_H_
#define OPERACIONES_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <commons/config.h>

int validarArchivo(int emisor);
void crearArchivo(int emisor);
void guardarDatos(int emisor);
char* obtenerDatos(int emisor);


#endif /*OPERACIONES_H_*/
