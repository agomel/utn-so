#ifndef PLP_H_
#define PLP_H_


#include <commons/collections/queue.h>
#include <commons/collections/list.h>
#include "planificador.h"
#include "S-AFA.h"

void planificadorALargoPlazo();

void ponerProcesoEnNew(char* escriptorio);

void enviarDTB(DTB proceso);

void ponerEnReady(DTB* dtb);

void pasarDTBAExit(int idDTB, t_list* listaDeDTB);

#endif /*PLP_H_*/
