#include "pcp.h"

void planificadorACortoPlazo(){
	u_int32_t a = 1;
	while(a){
		if(!list_is_empty(colaREADY)){
			//algoritmo para elegir
			int indexElegido = 0;
			//El list remove hace un get y lo borra de la lista
			waitSem(&cantidadTotalREADY);
			waitMutex(&mutexREADY);
			DTB* dtb = list_remove(colaREADY, indexElegido);
			signalMutex(&mutexREADY);

			serializarYEnviarDTB(socketCPU, *dtb);

			if(dtb->flag==0){
				//Es el dummy, lo desbloqueo para que se pueda volver a usar
				signalMutex(&mutexDummy);
			}
		}
	}
}


