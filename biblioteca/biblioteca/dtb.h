#ifndef DTB_H_
#define DTB_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "semaforos.h"
#include "serializacion.h"
#include "socket.h"

typedef struct{
	u_int32_t id;
	char* escriptorio;
	u_int32_t programCounter;
	u_int32_t flag;
	t_dictionary* direccionesArchivos;
	u_int32_t quantum;
	char estado;
}DTB;

DTB crearDTB (char* parametro);

int contadorIds;

pthread_mutex_t mutexIdsDTB;

void serializarYEnviarDTB(int receptor, DTB dtb);

DTB deserializarDTB(int emisor);

DTB* obtenerDTBDeColaRemoviendolo(t_list* cola, u_int32_t idDTB);

DTB* obtenerDTBDeCola(t_list* cola, u_int32_t idDTB);

void freeDTB(DTB* dtb);

#endif /*DTB_H_*/
