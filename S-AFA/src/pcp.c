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
	dtb->quantum = config_get_int_value(configuracion, "QUANTUM");
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
		waitSem(&gradoMultiprocesamiento);
		DTB* dtb = seleccionarDTB();
		cambiarEstado(dtb->id, EXECUTE);
		serializarYEnviarDTB(socketCPU, *dtb);
	}
}

void desbloquearDTB(DTB* dtb){
	if(dtb->flag == 0){
		signalMutex(&mutexDummy);
	}else{
		cambiarEstadoGuardandoNuevoDTB(dtb, READY);
	}
}

