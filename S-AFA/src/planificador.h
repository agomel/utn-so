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
#include <stdbool.h>

t_queue* colaNEW;
t_list* colaREADY;
t_list* colaEXECUTE;
t_list* colaBLOCKED;
t_list* colaEXIT;
t_list* colaEsperandoDummy;
t_list* listaDeTodosLosDTBs;

pthread_mutex_t mutexNEW;
pthread_mutex_t mutexREADY;
pthread_mutex_t mutexEXECUTE;
pthread_mutex_t mutexBLOCKED;
pthread_mutex_t mutexEXIT;
pthread_mutex_t mutexColaDummy;
pthread_mutex_t mutexListaDTBs;

sem_t gradoMultiprogramacion;
sem_t cantidadTotalREADY;
sem_t semCantidadEnNew;

//Habria que hacer un malloc para el dummy?
DTB* dtbDummy;
pthread_mutex_t mutexDummy;

t_config* configuracion;

void inicializarPlanificadores();

void inicializarColas();
void inicializarSemaforos();

t_list* filtrarListaPorEstado(char estado);
DTB* cambiarDTBDeColaBuscandoloEnListaDeTodos(DTB* dtb, t_list* nuevaLista);

t_list* obtenerColaSinNew(char estado);

#endif /*PLANIFICADOR_H_*/
