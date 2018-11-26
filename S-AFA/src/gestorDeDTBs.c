#include "gestorDeDTBs.h"


void loguearEstadoDTB(DTB* dtb){
	if(dtb->flag == 0){
		log_info(logger, "DUMMY");
	}else{
		log_info(logger, "id: %d, escriptiorio: %s, programCounter: %d, status: %s, quantum: %d, flag: %d",
					dtb->id, dtb->escriptorio, dtb->programCounter, nombreEstado(dtb->estado), dtb->quantum, dtb->flag);
	}
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

void operacionDelDiego(int idDTB){
	waitMutex(&mutexSentenciasDeDiego);
	sentenciasTotalesQueUsaronAlDiego++;
	signalMutex(&mutexSentenciasDeDiego);
	finalizarHistorialDeListaTiempoRespuesta(idDTB);

}

void agregarSentencias(int sentenciasEjecutadas){
	log_debug(logger, "Hay %d sentencias totales", sentenciasTotales);
	waitMutex(&mutexSentenciasTotales);
	sentenciasTotales += sentenciasEjecutadas;
	signalMutex(&mutexSentenciasTotales);
}

void agregarHistorialAListaNew(Historial* historial){
	log_debug(logger, "Agregando nuevo historial a new con llegada: %d y salida %d", historial->llegada, historial->salida);
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
	return list_find(lista, esHistorial);
}
void finalizarHistorialDeListaNew(int idDTB){
	waitMutex(&mutexHistorialNew);
	Historial* historial = encontrarHistorial(listaHistorialNew, idDTB);
	signalMutex(&mutexHistorialNew);

	waitMutex(&mutexSentenciasTotales);
	historial->salida = sentenciasTotales;
	signalMutex(&mutexSentenciasTotales);

	log_debug(logger, "Finalizo historial new con llegada: %d y salida %d", historial->llegada, historial->salida);

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

	log_info(logger, "-------------------------------------------");
	log_info(logger, "Cantidad de sentencias ejecutadas que espero el DTB con id %d en NEW : %d sentencias", idDTB, cantidadSentencias(historial));
}

int mostrarMetricasConDTBEXIT(int idDTB){
	Historial* obtenerPorId(Historial* historial){
		return historial->idDTB == idDTB;
	}
	waitMutex(&mutexHistorialExit);
	Historial* historial = list_find(listaHistorialExit, obtenerPorId);
	signalMutex(&mutexHistorialExit);
	log_info(logger, "-------------------------------------------");
	log_info(logger, "Cantidad de sentencias ejecutadas que espero el DTB con id %d para llegar a EXIT : %d sentencias", idDTB, cantidadSentencias(historial));

	return cantidadSentencias(historial);
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
	waitMutex(&mutexHistorialExit);
	int cantidadHistorialExit = listaHistorialExit->elements_count;
	signalMutex(&mutexHistorialExit);
	int sentenciasTotalesExit = 0;
	for(int i = 0; i < cantidadHistorialExit; i++){
		waitMutex(&mutexHistorialExit);
		Historial* historial = list_get(listaHistorialExit, i);
		signalMutex(&mutexHistorialExit);
		sentenciasTotalesExit += mostrarMetricasConDTBEXIT(historial->idDTB);
	}


	waitMutex(&mutexListaDTBs);
	int hayDTBs = listaDeTodosLosDTBs->elements_count;
	signalMutex(&mutexListaDTBs);
	if(hayDTBs){
		log_info(logger, "-------------------------------------------");
		log_info(logger, "La cantidad de sentencias ejecutadas prom. para que un DTB termine en la cola EXIT es %d", sentenciasTotalesExit / cantidadHistorialExit);
	}else{
		log_info(logger, "No hay metricas para mostrar");
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

	waitMutex(&mutexListaDTBs);
	int hayDTBs = listaDeTodosLosDTBs->elements_count;
	signalMutex(&mutexListaDTBs);

	if(hayDTBs){
		log_info(logger, "-------------------------------------------");
		log_info(logger, "La cantidad de sentencias ejecutadas prom. que usaron al diego son %d", totales / diego);

		log_info(logger, "-------------------------------------------");
		log_info(logger, "El porcentaje de sentencias ejecutadas prom. que usaron al diego son el %d", diego * 100 / totales);

	}else{
		log_info(logger, "-------------------------------------------");
		log_info(logger, "No hay metricas para mostrar");
	}

}

Historial* encontrarHistorialBloqueado(t_list* lista, int idDTB){
       bool esHistorial(Historial* historial){
               return idDTB == historial->idDTB && historial->salida == -1;
       }
       return list_find(lista, esHistorial);
}

void agregarHistorialAListaTiempoRespuesta(Historial* historial){
       waitMutex(&mutexHistorialBloqueados);
       list_add(listaHistorialBloqueados, historial);
       signalMutex(&mutexHistorialBloqueados);
}

void finalizarHistorialDeListaTiempoRespuesta(int idDTB){
       waitMutex(&mutexHistorialBloqueados);
       Historial* historial = encontrarHistorialBloqueado(listaHistorialBloqueados, idDTB);
       signalMutex(&mutexHistorialBloqueados);

       waitMutex(&mutexSentenciasTotales);
       historial->salida = sentenciasTotales;
       signalMutex(&mutexSentenciasTotales);
}

void mostrarMetricasTiempoDeRespuesta(){
       bool seDesbloqueo(Historial* historial){
               return historial->salida != -1;
       }
       waitMutex(&mutexHistorialBloqueados);

       t_list* listaDesbloqueados = list_filter(listaHistorialBloqueados, seDesbloqueo);
       signalMutex(&mutexHistorialBloqueados);

       int totalTiempo = 0;
       for(int i = 0; i < listaDesbloqueados->elements_count; i++){
    	   Historial* historial =  list_get(listaDesbloqueados, i);
    	   totalTiempo += cantidadSentencias(historial);
       }
       log_info(logger, "-------------------------------------------");
       log_info(logger, "El tiempo promedio de respuesta es: %d", totalTiempo / listaDesbloqueados->elements_count);
}


void mostrarMetricas(){
	mostrarSentenciasDeTodos();
	mostrarMetricasDelDIEGO();
	mostrarMetricasTiempoDeRespuesta();

}

void verificarSiExisteRecurso(char* recurso){
	waitMutex(&mutexRecursos);
	int tieneLaClave = dictionary_has_key(recursos, recurso);
	signalMutex(&mutexRecursos);
	if(!tieneLaClave){
		int cantidadRecurso = 1;
		waitMutex(&mutexRecursos);
		dictionary_put(recursos, recurso, cantidadRecurso);
		signalMutex(&mutexRecursos);
		log_info(logger, "Creado el recurso %s, no existia", recurso);
	}
}
char asignarRecurso(int idDTB, char* recurso){
	verificarSiExisteRecurso(recurso);
	waitMutex(&mutexRecursos);
	int cant = dictionary_remove(recursos, recurso);
	log_info(logger, "La cant de recursos que tengo es %d", cant);
	signalMutex(&mutexRecursos);
	char header;
	if(cant > 0){
		cant--;
		header = CONTINUAR_CON_EJECUCION;
	}else{
		waitMutex(&mutexEsperandoRecursos);
		DTBEsperandoRecurso* der = asignarMemoria(sizeof(DTBEsperandoRecurso));
		der->idDTB = idDTB;
		der->recurso = recurso;
		list_add(esperandoRecursos, der);
		signalMutex(&mutexEsperandoRecursos);
		log_info(logger, "No esta el recurso disponible.. vas a pasar a bloqueado :( DTB: %d", idDTB);
		header =  LIBERAR_DTB_DE_EJECUCION;
	}

	waitMutex(&mutexRecursos);
	dictionary_put(recursos, recurso, cant);
	signalMutex(&mutexRecursos);
	return header;
}
char liberarRecurso(char* recurso){

	bool esQuienEspera(DTBEsperandoRecurso* esperando){
		return !strcmp(esperando->recurso, recurso);
	}

	verificarSiExisteRecurso(recurso);


	waitMutex(&mutexEsperandoRecursos);
	t_list* hayEsperandoEsteRecurso = list_filter(esperandoRecursos, esQuienEspera);
	signalMutex(&mutexEsperandoRecursos);


	if(hayEsperandoEsteRecurso->elements_count > 0){
		DTBEsperandoRecurso* waiteando = list_get(hayEsperandoEsteRecurso, 0);
		bool esElQueEspera(DTBEsperandoRecurso* esperando){
				return esperando->idDTB == waiteando->idDTB;
		}
		waitMutex(&mutexEsperandoRecursos);
		DTBEsperandoRecurso* espera = list_remove_by_condition(esperandoRecursos, esElQueEspera);
		signalMutex(&mutexEsperandoRecursos);

		Historial* historial = crearHistorial(espera->idDTB);
		agregarHistorialAListaTiempoRespuesta(historial);
		ponerEnReady(espera->idDTB);
	}else{
		//Le sumo uno a los recursos
		waitMutex(&mutexRecursos);
		int cant = dictionary_get(recursos, recurso);
		signalMutex(&mutexRecursos);
		cant++;
		waitMutex(&mutexRecursos);
		dictionary_put(recursos, recurso, cant);
		signalMutex(&mutexRecursos);
	}
	return CONTINUAR_CON_EJECUCION;


}
