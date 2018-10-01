#include "pcp.h"

void planificadorACortoPlazo(){
	u_int32_t a = 1;
	while(a){
		if(!queue_is_empty(&colaREADY)){
			waitSem(&cantidadTotalREADY);
			waitMutex(&mutexREADY);
			//algoritmo para elegir
			DTB* dtb = queue_pop(colaREADY);
			//enviar a cpu dtb a ejecutar
			//si es el dummy
			//desblockiar dummy
			signalMutex(&mutexREADY);
			signalSem(&gradoMultiprogramacion);
			//TODO ejecutar el dtb
		}
	}
}


