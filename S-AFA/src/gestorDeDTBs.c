#include "gestorDeDTBs.h"


void loguearEstadoDTB(DTB* dtb){
	log_info(logger, "id: %d, escriptiorio: %s, programCounter: %d, status: %s, quantum: %d, flag: %d",
			dtb->id, dtb->escriptorio, dtb->programCounter, nombreEstado(dtb->estado), dtb->quantum, dtb->flag);
}

void loguearEstadoDeLista(t_list* lista, char estado){
	log_info(logger, "-------------------------------");
	log_info(logger, "Estado de lista: %s", nombreEstado(estado));
	log_info(logger, "Cantidad de DTBs en lista: %d", lista->elements_count);
	if(lista->elements_count > 0) log_info(logger, "DTBs en lista:");
	for(int i = 0; i<lista->elements_count; i++){
		loguearEstadoDTB(list_get(lista, i));
	}
	list_destroy(lista);
}
void mostrarStatus(){
	t_list* listaNews = filtrarListaPorEstado(NEW);
	loguearEstadoDeLista(listaNews, NEW);
	t_list* listaReady = filtrarListaPorEstado(READY);
	loguearEstadoDeLista(listaReady, READY);
	t_list* listaBlocked = filtrarListaPorEstado(BLOCKED);
	loguearEstadoDeLista(listaBlocked, BLOCKED);
	t_list* listaExit = filtrarListaPorEstado(EXIT);
	loguearEstadoDeLista(listaExit, EXIT);
	t_list* listaExecute = filtrarListaPorEstado(EXECUTE);
	loguearEstadoDeLista(listaExecute, EXECUTE);
	t_list* listaReadyPrioridad = filtrarListaPorEstado(READY_PRIORIDAD);
	loguearEstadoDeLista(listaReadyPrioridad, READY_PRIORIDAD);
}



Historial* crearHistorial(int idDTB){
	Historial* historial = asignarMemoria(sizeof(Historial));
	historial->idDTB = idDTB;
	//historial->sentenciasNew= 0;
	//historial->sentenciasExit = 0;
	return historial;
}

void initGestorDTBs(){
	inicializarMutex(&mutexHistorial);
	listaHistorial = list_create();
}

void mostrarMetricasConDTB(int idDTB){
	Historial* obtenerPorId(Historial* historial){
		return historial->idDTB == idDTB;
	}
	waitMutex(&mutexHistorial);
	Historial* historial = list_find(listaHistorial, obtenerPorId);
	signalMutex(&mutexHistorial);

	//log_info("Cantidad de sentencias ejecutadas que espero el DTB con id %d en NEW : %d sentencias", idDTB, historial->sentenciasNew);
}
void mostrarSentenciasEnNewDeTodos(){
	waitMutex(&mutexHistorial);
	int cantidadHistorial = listaHistorial->elements_count;
	signalMutex(&mutexHistorial);
	for(int i = 0; i < cantidadHistorial; i++){
		waitMutex(&mutexHistorial);
		Historial* historial = list_get(listaHistorial, i);
		signalMutex(&mutexHistorial);
		mostrarMetricasConDTB(historial->idDTB);
	}
}
void mostrarMetricas(){
	mostrarSentenciasEnNewDeTodos();
	//TODO hacer metricas

}
