#include "plp.h"

void planificadorALargoPlazo(){
	u_int32_t a = 1;
	while(a){
		if(!queue_is_empty(colaNEW)){
			waitMutex(mutexNEW);
			DTB* dtb = queue_pop(colaNEW);
			signalMutex(mutexNEW);
			//TODO operacion dummmy
			waitSem(espacioDisponibleREADY);
			waitMutex(mutexREADY);
			queue_push(colaREADY, dtb);
			signalMutex(mutexREADY);
			signalSem(cantidadTotalREADY);
		}
	}
}



