#include "plp.h"

void planificadorALargoPlazo(){
	u_int32_t a = 1;
	while(a){
		if(!queue_is_empty(colaNEW)){
			printf("hay procesos en la cola new\n");
			waitMutex(&mutexNEW);
			DTB* dtb = queue_pop(colaNEW);
			//Quizas seria un peek o no se que
			signalMutex(&mutexNEW);

			waitMutex(&mutexDummy);
			//poner en la cola de ready el DTBdummy
			//block dummy
			//enviarOperacionDummy(*dtb);
			signalMutex(&mutexDummy);
			waitSem(&espacioDisponibleREADY);
			waitMutex(&mutexREADY);
			queue_push(colaREADY, dtb);
			signalMutex(&mutexREADY);
			signalSem(&cantidadTotalREADY);
		}
	}
}

/*void dtbListo(DTBListo datos){
	//Busco en la lista el dtb con el id de datos dtb
	DTB dtb;
	waitSem(&espacioDisponibleREADY);
	waitMutex(&mutexREADY);
	queue_push(colaREADY, dtb);
	signalMutex(&mutexREADY);
	signalSem(&cantidadTotalREADY);
}*/
void enviarOperacionDummy(DTB dtb){
	dtbDummy.flag = 1;
	dtbDummy.escriptorio = dtb.escriptorio;
	dtbDummy.id = dtb.id;
	serializarYEnviarDTB(socketCPU, dtbDummy);
}

void ponerProcesoEnNew(char* escriptorio){
	DTB* proceso = asignarMemoria(sizeof(DTB));
	*proceso = crearDTB(escriptorio);
	waitMutex(&mutexNEW);
	queue_push(colaNEW, proceso);
	signalMutex(&mutexNEW);
}

void enviarDTB(DTB proceso){
	serializarYEnviarDTB(socketCPU, proceso);
}
