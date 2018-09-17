#include "plp.h"

void planificadorALargoPlazo(){
	u_int32_t a = 1;
	while(a){
		if(!queue_is_empty(colaNEW)){
			waitMutex(mutexNEW);
			DTB* dtb = queue_pop(colaNEW);
			signalMutex(mutexNEW);
			realizarProcesoDummy(*dtb);
			waitSem(espacioDisponibleREADY);
			waitMutex(mutexREADY);
			queue_push(colaREADY, dtb);
			signalMutex(mutexREADY);
			signalSem(cantidadTotalREADY);
		}
	}
}

void ponerProcesoEnNew(char* escriptorio){
	DTB proceso = crearDTB(escriptorio);
	waitMutex(mutexNEW);
	queue_pop(&proceso);
	signalMutex(mutexNEW);
}

void realizarProcesoDummy(DTB proceso){
	//TODO hacer el proceso dummy
}

