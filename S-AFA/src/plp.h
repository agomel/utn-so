#ifndef PLP_H_
#define PLP_H_


#include <commons/collections/queue.h>
#include <commons/collections/list.h>
#include "planificador.h"
#include "S-AFA.h"

void planificadorALargoPlazo();

void ponerProcesoEnNew(char* escriptorio);

void enviarDTB(DTB proceso);
//TODO no recibe int recibe la estructura de guardado
void ponerEnReadyProcesoDummyOk(DTB* dtb);

#endif /*PLP_H_*/
