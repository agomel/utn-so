#ifndef MDJ_H_
#define MDJ_H_

#include <stdio.h>
#include <stdlib.h>
#include <biblioteca/utilidades.h>
#include <biblioteca/socket.h>
#include <biblioteca/select.h>
#include <biblioteca/hilos.h>
#include "operaciones.h"
#include <commons/collections/queue.h>
#include <biblioteca/semaforos.h>
#include <biblioteca/nuestroSelect.h>
#include <biblioteca/traductor.h>
#include "consola.h"

int RETARDO;
char* PUNTO_MONTAJE;
char* MONTAJE_ACTUAL;

int TAMANIO_BLOQUES;
int CANTIDAD_BLOQUES;
char* MAGIC_NUMBER;

t_queue* colaOperaciones;
pthread_mutex_t mutexOperaciones;
sem_t semOperaciones;
sem_t semProductores;

t_log* logger;

int identificarse(int emisor, char header);

#endif /*MDJ_H_*/
