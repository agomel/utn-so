#ifndef GESTORDEDTBS_H_
#define GESTORDEDTBS_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "S-AFA.h"
#include <commons/collections/list.h>


typedef struct{
	int idDTB;
	int sentenciasNew;
	int sentenciasExit;
}Historial;

//lista id llegada salida

Historial* crearHistorial(int idDTB);
t_list* listaHistorial;
pthread_mutex_t mutexHistorial;

void initGestorDTBs();

#endif /*GESTORDEDTBS_H_*/

