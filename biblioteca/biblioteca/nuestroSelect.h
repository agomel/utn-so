#ifndef NUESTROSELECT_H_
#define NUESTROSELECT_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <commons/collections/queue.h>
#include "socket.h"
#include "hilos.h"
#include "utilidades.h"
#include "semaforos.h"

typedef struct{
	int socket;
	sem_t* semOperaciones;
	t_log* logger;
	pthread_mutex_t* mutexOperaciones;
	t_queue* colaOperaciones;
	void (*funcionEntenderMensaje)(int, char);
	int (*identificarse)(int, char);
}Select;

typedef struct{
	Select* select;
	int conectado;
}SocketEnSelect;

#endif /*NUESTROSELECT_H_*/
