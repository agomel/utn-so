#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <biblioteca/dtb.h>
#include <commons/collections/queue.h>
#include <biblioteca/semaforos.h>

t_queue* colaNEW;
t_queue* colaREADY;
t_queue* colaEXECUTE;
t_queue* colaBLOCKED;

pthread_mutex_t* mutexNEW;
pthread_mutex_t* mutexREADY;

sem_t* espacioDisponibleREADY;
sem_t* cantidadTotalREADY;


void inicializarColas();

void inicializarSemaforos();

#endif /*PLANIFICADOR_H_*/
