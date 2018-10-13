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
	sem_t* semProductores;
}Select;

typedef struct{
	Select* select;
	int conectado;
}SocketEnSelect;

void realizarNuestroSelect(Select* select);
void aceptarClientes(Select* select);
void consumirCola(Select* select);
void escucharCliente(SocketEnSelect* socketEnSelect);
void agregarPedidoACola(char header,SocketEnSelect* socketEnSelect);
void freeSelect(Select* select);

#endif /*NUESTROSELECT_H_*/
