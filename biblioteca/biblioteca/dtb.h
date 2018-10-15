#ifndef DTB_H_
#define DTB_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "semaforos.h"
#include "serializacion.h"
#include "socket.h"
#include "logger.h"

typedef struct{
	int id;
	char* escriptorio;
	int programCounter;
	int flag;
	t_dictionary* direccionesArchivos;
	int quantum;
	char estado;
}DTB;

DTB crearDTB (char* parametro);

int contadorIds;

pthread_mutex_t mutexIdsDTB;

void serializarYEnviarDTB(int receptor, DTB dtb, t_log* logger, char operacion);

DTB deserializarDTB(int emisor);

void freeDTB(DTB* dtb);

#endif /*DTB_H_*/
