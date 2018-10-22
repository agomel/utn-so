#ifndef GESTORDEDTBS_H_
#define GESTORDEDTBS_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "S-AFA.h"
#include <commons/collections/list.h>


typedef struct{
	int idDTB;
	int llegada;
	int salida;
}Historial;

/*int sentenciasTotalesQueUsaronAlDiego;
pthread_mutex_t mutexSentenciasDeDiego;

t_list* listaHistorialBloqueados;
pthread_mutex_t mutexHistorialBloqueados;

t_list* listaHistorialNew;
pthread_mutex_t mutexHistorialNew;

t_list* listaHistorialExit;
pthread_mutex_t mutexHistorialExit;

Historial* crearHistorial(int idDTB);

void initGestorDTBs();
*/

#endif /*GESTORDEDTBS_H_*/

