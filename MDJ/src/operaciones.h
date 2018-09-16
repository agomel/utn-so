#ifndef OPERACIONES_H_
#define OPERACIONES_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <commons/config.h>

u_int32_t validarArchivo(int emisor);
void crearArchivo(int emisor);
char* obtenerDatos(u_int32_t emisor);


#endif /*OPERACIONES_H_*/
