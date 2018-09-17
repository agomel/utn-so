#ifndef DTB_H_
#define DTB_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "semaforos.h"

typedef struct{
	u_int32_t id;
	char* escriptorio;
	u_int32_t programCounter;
	u_int32_t flag;
	u_int32_t tablaDireccionesArchivos;
}DTB;

DTB crearDTB (char* parametro);

int contadorIds;

pthread_mutex_t mutexIdsDTB;



#endif /*DTB_H_*/
