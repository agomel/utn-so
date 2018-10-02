#ifndef UTILIDADES_H_
#define UTILIDADES_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "socket.h"
#include <stdbool.h>
#include <commons/config.h>

#define ARCHIVO_CONFIGURACION "configuraciones/configuracion.txt"
#define ARCHIVO_LOG "../../log.txt"

enum OPERACIONES{
	IDENTIFICARSE = 'a', MANDAR_TEXTO = 'b', MANDAR_MENSAJITO = 'c', CARGAR_ESCRIPTORIO = 'd',
	VALIDAR_ARCHIVO = 'e', CREAR_ARCHIVO = 'f', OBTENER_DATOS = 'g', GUARDAR_DATOS = 'h', ENVIAR_DTB = 'i',
	DATOS_CONSEGUIDOS = 'j', DTB_LISTO = 'k' , RESPUESTA_CARGA = 'l', FALLO_LA_CARGA_DEL_SCRIPTORIO = 'm', PASAR_READY = 'n'
};

enum MODULOS{
	SAFA='s', CPU='c', MDJ='m', FM9='f', DAM='d'
};

enum COLAS{
	COLA_NEW = 'n', COLA_READY = 'r', COLA_EXEC = 'e'
};

void* asignarMemoria(int cantidad);


typedef struct{
	char* ip;
	int puerto;
}direccionServidor;

direccionServidor levantarDeConfiguracion(char* nombreIp, char* nombrePuerto, char* rutaArchivo);

typedef struct {
		int servidor;
		void* funcion;
	} parametrosEscucharClientes;

int escucharClientes(parametrosEscucharClientes* parametros);


typedef struct{
	char* texto1;
	int numero;
	char* texto2;
	char caracter;
}mensajito;

#endif /*UTILIDADES_H_*/
