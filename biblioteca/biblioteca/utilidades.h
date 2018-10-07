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
	IDENTIFICARSE = 'a', MANDAR_TEXTO = 'b', CARGAR_ESCRIPTORIO = 'c',
	VALIDAR_ARCHIVO = 'd', CREAR_ARCHIVO = 'e', OBTENER_DATOS = 'f', GUARDAR_DATOS = 'g', ENVIAR_DTB = 'h',
	DATOS_CONSEGUIDOS = 'i', DTB_LISTO = 'j' , RESPUESTA_CARGA = 'k',
	FALLO_LA_CARGA_DEL_SCRIPTORIO = 'l', PASAR_READY = 'm', DESBLOQUEAR_DTB = 'n', BLOQUEAR_DTB = 'o'
};

enum MODULOS{
	SAFA='s', CPU='c', MDJ='m', FM9='f', DAM='d'
};

enum COLAS{
	COLA_NEW = 'n', COLA_READY = 'r', COLA_EXEC = 'e'
};

enum ESTADO{
	NEW = 'a', READY = 'b', BLOCKED = 'c', EXIT = 'd', EXECUTED = 'e', ESPERANDO_DUMMY = 'f'
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

#endif /*UTILIDADES_H_*/
