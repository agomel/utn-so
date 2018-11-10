#include "pcp.h"

DTB* planificarPorFIFO(){
	t_list* listaReady = filtrarListaPorEstado(READY);
	DTB* dtb = list_get(listaReady, 0);
	list_destroy(listaReady);
	dtb->quantum = -1;
	return dtb;
}

DTB* planificarPorRR(){
	t_list* listaReady = filtrarListaPorEstado(READY);
	DTB* dtb = list_get(listaReady, 0);
	list_destroy(listaReady);
	dtb->quantum = quantum;
	return dtb;
}

DTB* planificarPorVRR(){
	t_list* listaVRR = filtrarListaPorEstado(READY_PRIORIDAD);

	if(listaVRR->elements_count > 0){
		DTB* dtb = list_get(listaVRR, 0);
		list_destroy(listaVRR);
		return dtb;
	}else{
		return planificarPorRR();
	}
}

DTB* seleccionarDTB(){
	if(!strcmp(algoritmo, "FIFO")){
		return planificarPorFIFO();
	}else if(!strcmp(algoritmo, "RR")){
		return planificarPorRR();
	}else if(!strcmp(algoritmo, "VRR")){
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
			log_info(logger, "Pasado Dummy a ejecutar con escriptorio %s", dtb->escriptorio);
		}else{
			cambiarEstado(dtb->id, EXECUTE);
		}
		printf("enviando a ejecutar dtb con id %d \n", dtb->id);

		int socketCPU = obtenerCPUDisponibleYOcupar(dtb->id);
		log_debug(logger, "enviando a socket %d header %c", socketCPU, ENVIAR_DTB);
		serializarYEnviarDTB(socketCPU, *dtb, logger, ENVIAR_DTB);
	}
}

void desbloquearDTB(int idDTB){
	if(!strcmp(algoritmo, "VRR")){
		cambiarEstado(idDTB, READY_PRIORIDAD);
		signalSem(&cantidadTotalREADY);
	}else{
		ponerEnReady(idDTB);
	}
}

void desbloquearDummy(DTB* dummy){
	cambiarEstadoDummy(BLOCKED);
	signalMutex(&mutexDummy);
}

