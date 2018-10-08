#ifndef PLP_H_
#define PLP_H_


#include <commons/collections/queue.h>
#include <commons/collections/list.h>
#include "planificador.h"
#include "S-AFA.h"

void planificadorALargoPlazo();

void ponerProcesoEnNew(char* escriptorio);

void cargarDummy(DTB dtb);

void enviarDTB(DTB proceso);

void ponerEnReady(int dtb);

void pasarDTBAExit(int idDTB);

void manejarErrores(int idDTB,char* path,int error);

#endif /*PLP_H_*/
