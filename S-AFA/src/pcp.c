#include "pcp.h"

void planificadorACortoPlazo(){
	u_int32_t a = 1;
	while(a){
		waitSem(&cantidadTotalREADY);
		if(!list_is_empty(colaREADY)){
			printf("tamanio cola ready %d \n",colaREADY->elements_count);
			//algoritmo para elegir
			int indexElegido = 0;
			waitMutex(&mutexREADY);
			DTB* dtb;
			dtb = list_remove(colaREADY, 0);
			dtb->quantum = -1;
			signalMutex(&mutexREADY);

			dtb->estado = EXECUTED;
			serializarYEnviarDTB(socketCPU, *dtb);
		}
	}
}


void pasarDTBAReadyDesdeBlocked(DTB* dtb){
	obtenerDTBDeColaRemoviendolo(colaBLOCKED, dtb->id);
	list_add(colaREADY, dtb);
	dtb->estado = READY;
}

void bloquearDTB(DTB* dtb){
	cambiarDTBDeColaBuscandoloEnListaDeTodos(dtb, colaBLOCKED);
	dtb->estado = BLOCKED;
}


void desbloquearDTB(DTB* dtb){
	if(dtb->flag == 0){
		signalMutex(&mutexDummy);
	}else{
		pasarDTBAReadyDesdeBlocked(dtb);
	}
}

