#ifndef OPERACIONESFIFA_H_
#define OPERACIONESFIFA_H_

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
#include "operacionesArchivos.h"
#include "fileSystem.h"
#include <commons/string.h>

int validarArchivoFIFA(char* rutaArchivo);
int crearArchivoFIFA(char* rutaArchivo, int cantidadDeBytes);
char* obtenerDatosFIFA(char* rutaArchivo, int offset, int tamanioALeer);
int guardarDatosFIFA(char* rutaArchivo, int offset, int tamanioMensaje, char* datos);
int borrarArchivoFIFA(char* rutaArchivo);

extern t_log* logger;

extern char* PUNTO_MONTAJE_ARCHIVOS;

#endif /*OPERACIONESFIFA_H_*/
