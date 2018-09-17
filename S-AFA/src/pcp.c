#include "pcp.h"

void planificadorACortoPlazo(){
	u_int32_t a = 1;
	while(a){
		if(!queue_is_empty(colaREADY)){
			waitSem(cantidadTotalREADY);
			waitMutex(mutexREADY);
			DTB* dtb = queue_pop(colaREADY);
			signalMutex(mutexREADY);
			signalSem(espacioDisponibleREADY);
			//TODO ejecutar el dtb
		}
	}
}


