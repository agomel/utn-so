#ifndef UTILIDADES_H_
#define UTILIDADES_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#define ARCHIVO_CONFIGURACION "../configuraciones/configuracion.txt"
#define ARCHIVO_LOG "../../log.txt"

enum{
	identificarse=0, mandarTexto=1
};
void* asignarMemoria(int cantidad);

#endif /*UTILIDADES_H_*/
