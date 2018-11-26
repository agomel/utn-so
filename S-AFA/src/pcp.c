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
		if(dtb->quantum == 0){
			log_error(logger,"No deberia llegar aca");
			cambiarEstado(dtb->id, READY);
			return planificarPorVRR();
		}
		return dtb;
	}else{
		return planificarPorRR();
	}
}

DTB* planificarPorBOAF(){
	DTB* dtb = obtenerDTBConArchivoMasGrande();
	dtb->quantum = quantum;
	return dtb;
}

DTB* seleccionarDTB(){
	if(!strcmp(algoritmo, "FIFO")){
		return planificarPorFIFO();
	}else if(!strcmp(algoritmo, "RR")){
		return planificarPorRR();
	}else if(!strcmp(algoritmo, "VRR")){
		return planificarPorVRR();
	}else if(!strcmp(algoritmo, "BOAF")){
		return planificarPorBOAF();
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
		printf("enviando a ejecutar dtb con id %d y quantum %d\n", dtb->id, dtb->quantum);

		int socketCPU = obtenerCPUDisponibleYOcupar(dtb->id);
		serializarYEnviarDTB(socketCPU, *dtb, logger, ENVIAR_DTB);
	}
}

void desbloquearDTB(int idDTB){
	DTB* dtb = obtenerDTBDeCola(idDTB);
	if(dtb->estado != EXIT){
		if(!strcmp(algoritmo, "VRR") && dtb->quantum != 0){
				cambiarEstado(idDTB, READY_PRIORIDAD);
				signalSem(&cantidadTotalREADY);
			}else{
				ponerEnReady(idDTB);
			}
	}
}
void desbloquearDTBCambiandolo(DTB* dtb){
	if(dtb->estado != EXIT){
		if(!strcmp(algoritmo, "VRR") && dtb->quantum != 0){
				cambiarEstadoGuardandoNuevoDTB(dtb, READY_PRIORIDAD);
				signalSem(&cantidadTotalREADY);
			}else{
				ponerEnReadyDTB(dtb);
			}
	}
}

void desbloquearDummy(DTB* dummy){
	cambiarEstadoDummy(BLOCKED);
	signalSem(&semDummy);
}

