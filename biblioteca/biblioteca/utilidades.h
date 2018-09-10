#ifndef UTILIDADES_H_
#define UTILIDADES_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "socket.h"

#define ARCHIVO_CONFIGURACION "../configuraciones/configuracion.txt"
#define ARCHIVO_LOG "../../log.txt"

enum{
	identificarse=1, mandarTexto=2
};
void* asignarMemoria(int cantidad);

char** deserializar(int emisor);

#endif /*UTILIDADES_H_*/
