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
t_dictionary* conexiones;
int socketCPU;
int socketDAM;
char* storage;
pthread_mutex_t mutexStorage;
int offset;
pthread_mutex_t mutexOffset;
t_log* logger;

t_queue* colaOperaciones;
pthread_mutex_t mutexOperaciones;
sem_t semOperaciones;
sem_t semProductores;

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

void init();
void entenderMensaje(int emisor, char header);
int main(void);

#endif /*FM9_H_*/
