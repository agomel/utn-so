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
#include <commons/string.h>

#define ARCHIVO_CONFIGURACION "configuraciones/configuracion.txt"
#define ARCHIVO_LOG "log.txt"

enum OPERACIONES{
	IDENTIFICARSE = 'a', MANDAR_TEXTO = 'b', MANDAR_MENSAJITO = 'c', CARGAR_ESCRIPTORIO_EN_MEMORIA = 'd',
	VALIDAR_ARCHIVO = 'e', CREAR_ARCHIVO = 'f', OBTENER_DATOS = 'g', GUARDAR_DATOS = 'h', ENVIAR_DTB = 'i',
	DATOS_CONSEGUIDOS = 'j', DTB_LISTO = 'k' , RESPUESTA_CARGA = 'l', GUARDAR_ESCRIPTORIO = 'm', CARGADO_CON_EXITO_EN_MEMORIA = 'n',
	ERROR = 'o', DESBLOQUEAR_DTB = 'p', BLOQUEAR_DTB = 'q', TRAER_LINEA_ESCRIPTORIO = 'r', PASAR_A_EXIT = 's', TERMINO_QUANTUM = 't',
	ERROR_O_ACCESO_INVALIDO = 'u', FIN_ARCHIVO = 'v', GUARDADO_CON_EXITO_EN_MDJ = 'w', RETENCION_DE_RECURSO = 'x', LIBERAR_DTB_DE_EJECUCION = 'y',
	LIBERAR_RECURSO = 'z', LIBERAR_MEMORIA = '0', BORRAR_DATOS = '1'
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

typedef struct{
	int socket;
	char header;
}OperacionSocket;

void* asignarMemoria(int cantidad);


typedef struct{
	char* ip;
	int puerto;
}direccionServidor;

direccionServidor levantarDeConfiguracion(char* nombreIp, char* nombrePuerto, t_config* configuracion);

typedef struct {
		int servidor;
		void* funcion;
		t_log* logger;
	} parametrosEscucharClientes;

int escucharClientes(parametrosEscucharClientes* parametros);

char* intToString(int numero);

#endif /*UTILIDADES_H_*/
