#ifndef FM9_H_
#define FM9_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <biblioteca/utilidades.h>
#include <biblioteca/socket.h>
#include <biblioteca/select.h>
#include <biblioteca/hilos.h>
#include <commons/collections/list.h>
#include <biblioteca/logger.h>
#include <commons/collections/queue.h>
#include <biblioteca/semaforos.h>
#include <biblioteca/nuestroSelect.h>
#include <biblioteca/traductor.h>
#include <commons/string.h>
#include "manejoLineas.h"
#include <biblioteca/serializacion.h>

typedef struct{
	int pudoGuardar;
	int pesoArchivo;
}RespuestaGuardado;

typedef struct{
	char* datos;
	int pudoObtener;
}respuestaDeObtencionDeMemoria;

int socketCPU;
int socketDAM;
int tamanioMemoria;
int tamanioPagina;
int cantidadMarcosTotales;
int tamanioLinea;
void* storage;
int offset;
t_log* logger;
char* modo;
t_queue* colaOperaciones;
pthread_mutex_t mutexOperaciones;
pthread_mutex_t mutexListaSegmentos;
pthread_mutex_t mutexListaProcesos;
pthread_mutex_t mutexListaPaginas;
sem_t semProductores;
sem_t semOperaciones;
t_config* configuracion;

void entenderMensaje(int emisor, char header);

#endif /*FM9_H_*/
