#include "pcp.h"

DTB* planificarPorFIFO(){
	t_list* listaReady = filtrarListaPorEstado(READY);
	DTB* dtb = list_get(listaReady, 0);
	dtb->quantum = 0;
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
		log_info(logger, "Wait cantidadTotalEnReady");
		waitSem(&cantidadTotalREADY);
		log_info(logger, "Wait gradoMultiprocesamiento");
		waitSem(&gradoMultiprocesamiento);
		log_info(logger, "Paso gradoMultiprocesamiento");
		DTB* dtb = seleccionarDTB();
		if(dtb->flag == 0){
			cambiarEstadoDummy(EXECUTE);
			log_info(logger, "Pasado Dummy a ejecutar con scriptorio %s", dtb->escriptorio);
		}else{
			cambiarEstado(dtb->id, EXECUTE);
		}
		printf("enviando a ejecutar dtb con id %d \n", dtb->id);

		int socketCPU = obtenerCPUDisponibleYOcupar(dtb->id);
		serializarYEnviarDTB(socketCPU, *dtb, logger, ENVIAR_DTB);
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

