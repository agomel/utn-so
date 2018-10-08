#ifndef UTILIDADES_H_
#define UTILIDADES_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "socket.h"
#include <stdbool.h>
#include <commons/config.h>
#include "logger.h"

#define ARCHIVO_CONFIGURACION "configuraciones/configuracion.txt"
#define ARCHIVO_LOG "log.txt"

enum OPERACIONES{
	IDENTIFICARSE = 'a', MANDAR_TEXTO = 'b', MANDAR_MENSAJITO = 'c', CARGAR_ESCRIPTORIO_EN_MEMORIA = 'd',
	VALIDAR_ARCHIVO = 'e', CREAR_ARCHIVO = 'f', OBTENER_DATOS = 'g', GUARDAR_DATOS = 'h', ENVIAR_DTB = 'i',
	DATOS_CONSEGUIDOS = 'j', DTB_LISTO = 'k' , RESPUESTA_CARGA = 'l',
	FALLO_LA_CARGA_DEL_SCRIPTORIO = 'm', OK_CARGA_DEL_SCRIPTORIO = 'n', GUARDAR_ESCRIPTORIO = 'o', FALLO_EL_GUARDADO_EN_MDJ = 'p',
	GUARDADO_CON_EXITO_EN_MDJ = 'q', ERROR = 'r', DESBLOQUEAR_DTB = 's', BLOQUEAR_DTB = 't', TRAER_LINEA_ESCRIPTORIO = 'u'
};

enum MODULOS{
	SAFA='s', CPU='c', MDJ='m', FM9='f', DAM='d'
};

enum COLAS{
	COLA_NEW = 'n', COLA_READY = 'r', COLA_EXEC = 'e'
};

enum ESTADO{
	NEW = 'a', READY = 'b', BLOCKED = 'c', EXIT = 'd', EXECUTE = 'e', READY_PRIORIDAD = 'g'
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
		t_log* logger;
	} parametrosEscucharClientes;

int escucharClientes(parametrosEscucharClientes* parametros);

#endif /*UTILIDADES_H_*/
