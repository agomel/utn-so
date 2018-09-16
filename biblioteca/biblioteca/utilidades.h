#ifndef UTILIDADES_H_
#define UTILIDADES_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "socket.h"
#include <commons/config.h>
#define ARCHIVO_CONFIGURACION "configuraciones/configuracion.txt"
#define ARCHIVO_LOG "../../log.txt"

enum OPERACIONES{
	IDENTIFICARSE='a', MANDAR_TEXTO='b', MANDAR_MENSAJITO='c'
};

enum MODULOS{
	SAFA='s', CPU='c', MDJ='m', FM9='f', DAM='d'
};

void* asignarMemoria(int cantidad);


typedef struct{
	char* ip;
	int puerto;
}direccionServidor;

direccionServidor levantarDeConfiguracion(char* nombreIp, char* nombrePuerto, char* rutaArchivo);


typedef struct{
	char* texto1;
	int numero;
	char* texto2;
	char caracter;
}mensajito;

#endif /*UTILIDADES_H_*/
