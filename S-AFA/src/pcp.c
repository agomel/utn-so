#include "pcp.h"

DTB* planificarPorFIFO(){
	t_list* listaReady = filtrarListaPorEstado(READY);
	DTB* dtb = list_get(listaReady, 0);
	dtb->quantum = -1;
	return dtb;
}

DTB* planificarPorRR(){
	t_list* listaReady = filtrarListaPorEstado(READY);
	DTB* dtb = list_get(listaReady, 0);
	dtb->quantum = quantum;
	return dtb;
}

DTB* planificarPorVRR(){
	t_list* listaVRR = filtrarListaPorEstado(READY_PRIORIDAD);

	if(listaVRR->elements_count > 0){
		return list_get(listaVRR, 0);
	}else{
		return planificarPorRR();
	}
}

DTB* seleccionarDTB(){
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
		waitSem(&gradoMultiprocesamiento);
		DTB* dtb = seleccionarDTB();
		if(dtb->flag == 0){
			cambiarEstadoDummy(EXECUTE);
		}else{
			cambiarEstado(dtb->id, EXECUTE);
		}
		printf("enviando a ejecutar dtb con id %d \n", dtb->id);
		serializarYEnviarDTB(socketCPU, *dtb);
	}
}

void desbloquearDTB(DTB* dtb){
	if(dtb->flag == 0){
		cambiarEstadoDummy(BLOCKED);
		signalMutex(&mutexDummy);
	}else{
		if(strcmp(algoritmo, "VRR")){
			cambiarEstadoGuardandoNuevoDTB(dtb, READY_PRIORIDAD);
		}else{
			cambiarEstadoGuardandoNuevoDTB(dtb, READY);
		}
	}
}

