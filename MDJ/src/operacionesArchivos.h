#ifndef OPERACIONESFIFA_H_
#define OPERACIONESARCHIVOS_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <commons/config.h>
#include <biblioteca/logger.h>
#include <biblioteca/utilidades.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

int validarArchivo(char* rutaArchivo);
int crearArchivo(char* rutaArchivo);
char* obtenerDatos(char* rutaArchivo, int offset, int tamanioALeer);
int guardarDatos(char* rutaArchivo, int offset, int tamanioMensaje, char* datos);
int eliminarArchivo(char* rutaArchivo);

extern t_log* logger;


#endif /*OPERACIONESARCHIVOS_H_*/
