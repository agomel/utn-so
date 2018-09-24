#ifndef PLP_H_
#define PLP_H_


#include <commons/collections/queue.h>
#include "planificador.h"
#include "S-AFA.h"

void planificadorALargoPlazo();

void ponerProcesoEnNew(char* escriptorio);

void enviarDTB(DTB proceso);

#endif /*PLP_H_*/
