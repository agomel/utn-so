#include "plp.h"

void planificadorALargoPlazo(){
	u_int32_t a = 1;
	while(a){
		if(!queue_is_empty(colaNEW)){
			printf("b\n");
			waitMutex(&mutexNEW);
			DTB* dtb = queue_pop(colaNEW);
			signalMutex(&mutexNEW);
			enviarDTB(*dtb);
			waitSem(&espacioDisponibleREADY);
			waitMutex(&mutexREADY);
			queue_push(colaREADY, dtb);
			signalMutex(&mutexREADY);
			signalSem(&cantidadTotalREADY);
		}
	}
}

void ponerProcesoEnNew(char* escriptorio){
	DTB proceso = crearDTB(escriptorio);
	waitMutex(&mutexNEW);
	queue_push(colaNEW, &proceso);
	signalMutex(&mutexNEW);
}

void enviarDTB(DTB proceso){
	serializarYEnviarDTB(socketCPU, proceso, ENVIAR_DTB);
}

