#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <biblioteca/dtb.h>
#include <commons/collections/queue.h>
#include <commons/collections/list.h>
#include <biblioteca/semaforos.h>
#include <biblioteca/dtb.h>

t_queue* colaNEW;
t_list* colaREADY;
t_queue* colaEXECUTE;
t_queue* colaBLOCKED;
t_list* esperandoDummy;

pthread_mutex_t mutexColaDummy;
pthread_mutex_t mutexNEW;
pthread_mutex_t mutexREADY;

sem_t gradoMultiprogramacion;
sem_t cantidadTotalREADY;

//Habria que hacer un malloc para el dummy?
DTB dtbDummy;
pthread_mutex_t mutexDummy;

void inicializarPlanificadores();

void inicializarColas();
void inicializarSemaforos();

#endif /*PLANIFICADOR_H_*/
