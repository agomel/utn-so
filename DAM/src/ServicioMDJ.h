#ifndef SERVICIOMDJ_H_
#define SERVICIOMDJ_H_

#include <biblioteca/utilidades.h>
#include <biblioteca/serializacion.h>
#include <biblioteca/logger.h>

extern int socketMDJ;

extern int transferSize;

extern t_log* logger;

int validarArchivoMDJ(char* path);
char* obtenerDatosDeMDJ(int transferSize);
int crearArchivoEnMDJ(char* path, int cantidadDeLineas);
int guardarDatosEnMDJ(void* datosTotales);

#endif /*SERVICIOMDJ_H_*/
