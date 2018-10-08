#include "pcp.h"

DTB* planificarPorFIFO(){
	waitMutex(&mutexREADY);
	DTB* dtb = list_remove(colaREADY, 0);
	signalMutex(&mutexREADY);
	dtb->quantum = -1;
	return dtb;
}

DTB* planificarPorRR(){
	waitMutex(&mutexREADY);
	DTB* dtb = list_remove(colaREADY, 0);
	signalMutex(&mutexREADY);
	dtb->quantum = config_get_int_value(configuracion, "QUANTUM");
	return dtb;
}

DTB* planificarPorVRR(){
	waitMutex(&mutexListaDTBs);
	t_list* listaVRR = filtrarListaPorEstado(READY_PRIORIDAD);
	signalMutex(&mutexListaDTBs);
	if(listaVRR->elements_count > 0){
		return list_get(listaVRR, 0);
	}else{
		return planificarPorRR();
	}
}

void* seleccionarDTB(){
	char* algoritmo = config_get_string_value(configuracion, "ALGORITMO");
	if(strcmp(algoritmo, "FIFO")){
		return planificarPorFIFO();
	}else if(strcmp(algoritmo, "RR")){
		return planificarPorRR();
	}else if(strcmp(algoritmo, "VRR")){
		return planificarPorVRR();
	}
}
void planificadorACortoPlazo(){
	int a = 1;
	while(a){
		waitSem(&cantidadTotalREADY);
		if(!list_is_empty(colaREADY)){
			DTB* dtb = seleccionarDTB();
			dtb->estado = EXECUTED;
			//TODO hacer a cola de execute
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

