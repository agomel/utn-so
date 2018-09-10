#ifndef UTILIDADES_H_
#define UTILIDADES_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <commons/config.h>

#define ARCHIVO_CONFIGURACION "../configuraciones/configuracion.txt"
#define ARCHIVO_LOG "../../log.txt"

enum{
	identificarse=0, mandarTexto=1
};
void* asignarMemoria(int cantidad);

typedef struct{
	char* ip;
	int puerto;
}direccionServidor;

direccionServidor levantarDeConfiguracion(char* nombreIp, char* nombrePuerto, char* rutaArchivo);


#endif /*UTILIDADES_H_*/
