#include "plp.h"

void planificadorALargoPlazo(){
	u_int32_t a = 1;
	while(a){
		if(!queue_is_empty(colaNEW)){
			waitMutex(&mutexNEW);
			DTB* dtb = queue_pop(colaNEW);
			printf("Pude sacar de la cola\n");
			DTB dtb2 = *dtb;
			char* m = dtb2.escriptorio;
			printf("el escriptorio sacado de la cola es: %s\n", m);
			signalMutex(&mutexNEW);
			enviarDTB(dtb2);
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
	printf("el escriptorio fuera del dtb: %s\n", escriptorio);
	waitMutex(&mutexNEW);
	queue_push(colaNEW, &proceso);
	signalMutex(&mutexNEW);
	char* m = proceso.escriptorio;
	printf("el escriptorio es: %s\n", m);
}

void enviarDTB(DTB proceso){
	serializarYEnviarDTB(socketCPU, proceso, ENVIAR_DTB);
}
