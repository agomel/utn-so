#ifndef PCP_H_
#define PCP_H_

#include "planificador.h"
#include "S-AFA.h"

void planificadorACortoPlazo();

void pasarDTBAReadyDesdeBlocked(DTB* dtb);
void bloquearDTB(DTB* dtb);
void desbloquearDTB(DTB* dtb);


#endif /*PCP_H_*/
