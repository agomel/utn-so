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

int socketCPU;
int socketDAM;
void* storage;
int offset;
t_log* logger;
char* modo;
t_queue* colaOperaciones;
pthread_mutex_t mutexOperaciones;
sem_t semProductores;
sem_t semOperaciones;

typedef struct{
	t_list* listaDeDirecciones;
	int pudoGuardarlo;
}respuestaDeCargaEnMemoria;

typedef struct{
	char* datos;
	int cantidadDeLineas;
	int pudoGuardarlo;
}respuestaDeObtencionDeMemoria;

respuestaDeCargaEnMemoria cargarDatosEnMemoria(char* datos);
respuestaDeObtencionDeMemoria* obtenerDatosDeMemoria(t_list* posiciones);
void entenderMensaje(int emisor, char header);



#endif /*FM9_H_*/
