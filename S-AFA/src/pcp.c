#include "pcp.h"

void planificadorACortoPlazo(){
	u_int32_t a = 1;
	while(a){
		if(!list_is_empty(colaREADY)){
			waitSem(&cantidadTotalREADY);
			waitMutex(&mutexREADY);
			//algoritmo para elegir
			int indexElegido = 0;
			//El list remove hace un get y lo borra de la lista
			DTB* dtb = list_remove(colaREADY, indexElegido);
			serializarYEnviarDTB(socketCPU, *dtb);

			if(dtb->flag==0){
				signalMutex(&mutexDummy);
			}
			signalMutex(&mutexREADY);
			signalSem(&gradoMultiprogramacion);
		}
	}
}


