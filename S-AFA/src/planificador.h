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
#include "listas.h"
#include "S-AFA.h"
#include <biblioteca/traductor.h>

t_list* listaDeTodosLosDTBs;
pthread_mutex_t mutexListaDTBs;

sem_t gradoMultiprogramacion;
sem_t gradoMultiprocesamiento;
sem_t cantidadTotalREADY;
sem_t semCantidadEnNew;

//Habria que hacer un malloc para el dummy?
DTB* dtbDummy;
pthread_mutex_t mutexDummy;

int quantum;
char* algoritmo;

void inicializarPlanificadores();

void inicializarColas();
void inicializarSemaforos();

void cambiarEstado(int idDTB, char nuevoEstado);

void logguearCambioEstado(DTB* dtb, char nuevoEstado);

void cambiarEstadoGuardandoNuevoDTB(DTB* nuevoDTB, char nuevoEstado);

#endif /*PLANIFICADOR_H_*/
