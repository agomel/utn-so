#include "pcp.h"

void planificadorACortoPlazo(){
	u_int32_t a = 1;
	while(a){
		if(!list_is_empty(colaREADY)){
			printf("tamanio cola ready %d \n",colaREADY->elements_count);
			//algoritmo para elegir
			int indexElegido = 0;
			//El list remove hace un get y lo borra de la lista
			waitSem(&cantidadTotalREADY);
			waitMutex(&mutexREADY);
			DTB* dtb;
			dtb = list_remove(colaREADY, 0);
			dtb->quantum = -1;
			signalMutex(&mutexREADY);

			serializarYEnviarDTB(socketCPU, *dtb);

			if(dtb->flag==0){
				//Es el dummy, lo desbloqueo para que se pueda volver a usar
				signalMutex(&mutexDummy);
			}
		}
	}
}

void pasarDTBAReady(DTB* dtb){
	obtenerDTBDeColaRemoviendolo(colaBLOCKED, dtb->id);
	list_add(colaREADY, dtb);
}

void bloquearDTB(DTB* dtb){
	cambiarDTBDeColaBuscandoloEnListaDeTodos(dtb, colaBLOCKED);
}


void desbloquearDTB(DTB* dtb){
	if(dtb->flag == 0){
		signalMutex(&mutexDummy);
	}else{
		pasarDTBAReady(dtb);
	}
}

