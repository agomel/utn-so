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

void operacionDelDiego(){
	waitMutex(&mutexSentenciasDeDiego);
	sentenciasTotalesQueUsaronAlDiego++;
	signalMutex(&mutexSentenciasDeDiego);
}

void agregarSentencias(int sentenciasEjecutadas){
	waitMutex(&mutexSentenciasTotales);
	sentenciasTotales += sentenciasEjecutadas;
	signalMutex(&mutexSentenciasTotales);
}

void agregarHistorialAListaNew(Historial* historial){
	waitMutex(&mutexHistorialNew);
	list_add(listaHistorialNew, historial);
	signalMutex(&mutexHistorialNew);
}
void agregarHistorialAListaExit(Historial* historial){
	waitMutex(&mutexHistorialExit);
	list_add(listaHistorialExit, historial);
	signalMutex(&mutexHistorialExit);
}

Historial* encontrarHistorial(t_list* lista, int idDTB){
	bool esHistorial(Historial* historial){
		return idDTB == historial->idDTB;
	}
	list_find(lista, esHistorial);
}
void finalizarHistorialDeListaNew(int idDTB){
	waitMutex(&mutexHistorialNew);
	Historial* historial = encontrarHistorial(listaHistorialNew, idDTB);
	signalMutex(&mutexHistorialNew);

	waitMutex(&mutexSentenciasTotales);
	historial->salida = sentenciasTotales;
	signalMutex(&mutexSentenciasTotales);
}

void finalizarHistorialDeListaExit(int idDTB){
	waitMutex(&mutexHistorialExit);
	Historial* historial = encontrarHistorial(listaHistorialExit, idDTB);
	signalMutex(&mutexHistorialExit);

	waitMutex(&mutexSentenciasTotales);
	historial->salida = sentenciasTotales;
	signalMutex(&mutexSentenciasTotales);
}

Historial* crearHistorial(int idDTB){
	Historial* historial = asignarMemoria(sizeof(Historial));
	historial->idDTB = idDTB;
	waitMutex(&mutexSentenciasTotales);
	historial->llegada = sentenciasTotales;
	signalMutex(&mutexSentenciasTotales);
	historial->salida = -1;
	return historial;
}

void initGestorDTBs(){
	inicializarMutex(&mutexHistorialBloqueados);
	listaHistorialBloqueados = list_create();


	inicializarMutex(&mutexHistorialExit);
	listaHistorialExit = list_create();


	inicializarMutex(&mutexHistorialNew);
	listaHistorialNew = list_create();


	inicializarMutex(&mutexSentenciasDeDiego);
	sentenciasTotalesQueUsaronAlDiego = 0;


	inicializarMutex(&mutexSentenciasTotales);
	sentenciasTotales = 0;
}

int cantidadSentencias(Historial* historial){
	if(historial->salida != -1){
		return historial->salida - historial->llegada;
	}else{
		waitMutex(&mutexSentenciasTotales);
		int salida = sentenciasTotales;
		signalMutex(&mutexSentenciasTotales);
		return salida - historial->llegada;
	}
}


void mostrarMetricasConDTBNEW(int idDTB){
	Historial* obtenerPorId(Historial* historial){
		return historial->idDTB == idDTB;
	}
	waitMutex(&mutexHistorialNew);
	Historial* historial = list_find(listaHistorialNew, obtenerPorId);
	signalMutex(&mutexHistorialNew);

	log_info("Cantidad de sentencias ejecutadas que espero el DTB con id %d en NEW : %d sentencias", idDTB, cantidadSentencias(historial));
}

void mostrarMetricasConDTBEXIT(int idDTB){
	Historial* obtenerPorId(Historial* historial){
		return historial->idDTB == idDTB;
	}
	waitMutex(&mutexHistorialExit);
	Historial* historial = list_find(listaHistorialExit, obtenerPorId);
	signalMutex(&mutexHistorialExit);

	log_info("Cantidad de sentencias ejecutadas que espero el DTB con id %d en EXIT : %d sentencias", idDTB, cantidadSentencias(historial));
}

void mostrarSentenciasDeTodos(){
	//Muestro en new
	waitMutex(&mutexHistorialNew);
	int cantidadHistorialNew = listaHistorialNew->elements_count;
	signalMutex(&mutexHistorialNew);
	for(int i = 0; i < cantidadHistorialNew; i++){
		waitMutex(&mutexHistorialNew);
		Historial* historial = list_get(listaHistorialNew, i);
		signalMutex(&mutexHistorialNew);
		mostrarMetricasConDTBNEW(historial->idDTB);
	}

	//Muestro en exit
	waitMutex(&mutexHistorialNew);
	int cantidadHistorialExit = listaHistorialExit->elements_count;
	signalMutex(&mutexHistorialExit);
	for(int i = 0; i < cantidadHistorialExit; i++){
		waitMutex(&mutexHistorialExit);
		Historial* historial = list_get(listaHistorialExit, i);
		signalMutex(&mutexHistorialExit);
		mostrarMetricasConDTBEXIT(historial->idDTB);
	}
}
void mostrarMetricasConDTB(int idDTB){
	mostrarMetricasConDTBNEW(idDTB);
	mostrarMetricasConDTBEXIT(idDTB);
}

void mostrarMetricasDelDIEGO(){
	waitMutex(&mutexSentenciasTotales);
	int totales = sentenciasTotales;
	signalMutex(&mutexSentenciasTotales);


	waitMutex(&mutexSentenciasDeDiego);
	int diego = sentenciasTotalesQueUsaronAlDiego;
	signalMutex(&mutexSentenciasDeDiego);

	log_info(logger, "La cantidad de sentencias ejecutadas prom. que usaron al diego son el %d %", diego * 100 / totales);

}
void mostrarMetricas(){
	mostrarSentenciasDeTodos();
	mostrarMetricasDelDIEGO();

}
