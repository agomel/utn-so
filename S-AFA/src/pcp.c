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
			signalSem(&gradoMultiprogramacion);

			serializarYEnviarDTB(socketCPU, *dtb);

			if(dtb->flag==0){
				signalMutex(&mutexDummy);
			}

		}
	}
}


