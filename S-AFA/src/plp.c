#include "plp.h"

void planificadorALargoPlazo(){
	u_int32_t a = 1;
	while(a){
		if(!queue_is_empty(colaNEW)){
			printf("hay procesos en la cola new\n");
			waitMutex(&mutexNEW);
			DTB* dtb = queue_pop(colaNEW);
			signalMutex(&mutexNEW);

			waitMutex(&mutexDummy);
			enviarOperacionDummy(*dtb);
			signalMutex(&mutexDummy);

			enviarDTB(*dtb);
			waitSem(&espacioDisponibleREADY);
			waitMutex(&mutexREADY);
			queue_push(colaREADY, dtb);
			signalMutex(&mutexREADY);
			signalSem(&cantidadTotalREADY);
		}
	}
}

void enviarOperacionDummy(DTB dtb){
	dtbDummy.flag = 1;
	dtbDummy.escriptorio = dtb.escriptorio;
	dtbDummy.id = dtb.id;
	serializarYEnviarDTB(socketCPU, dtbDummy);
}

void ponerProcesoEnNew(char* escriptorio){
	DTB proceso = crearDTB(escriptorio);
	waitMutex(&mutexNEW);
	queue_push(colaNEW, &proceso);
	signalMutex(&mutexNEW);
}

void enviarDTB(DTB proceso){
	serializarYEnviarDTB(socketCPU, proceso);
}
