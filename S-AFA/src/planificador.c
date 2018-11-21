#include "planificador.h"
#include "S-AFA.h"

void inicializarPlanificadores(){
	inicializarColas();
	inicializarSemaforos();
	dtbDummy = asignarMemoria(sizeof(DTB));
	quantum = config_get_int_value(configuracion, "QUANTUM");
	char* punteroAlgoritmo = config_get_string_value(configuracion, "ALGORITMO");
	algoritmo = asignarMemoria(strlen(punteroAlgoritmo) + 1);
	memcpy(algoritmo, punteroAlgoritmo, strlen(punteroAlgoritmo)+ 1);
	log_info(logger, "Algoritmo elegido %s", algoritmo);
}

void inicializarColas(){
	listaDeTodosLosDTBs = list_create();
}

void inicializarSemaforos(){
	inicializarMutex(&mutexListaDTBs);
	inicializarMutex(&mutexDummy);

	int multiprogramacion = config_get_int_value(configuracion, "MULTIPROGRAMACION");
	inicializarSem(&gradoMultiprogramacion, multiprogramacion);
	inicializarSem(&gradoMultiprocesamiento, 0);
	inicializarSem(&cantidadTotalREADY, 0);
	inicializarSem(&semCantidadEnNew, 0);
}

void cambiarEstado(int idDTB, char nuevoEstado){
	DTB* dtb = obtenerDTBDeColaRemoviendolo(idDTB);
	dtb->estado = nuevoEstado;

	logguearCambioEstado(dtb, nuevoEstado);
	waitMutex(&mutexListaDTBs);
	list_add(listaDeTodosLosDTBs, dtb);
	signalMutex(&mutexListaDTBs);
}

void cambiarEstadoGuardandoNuevoDTB(DTB* nuevoDTB, char nuevoEstado){
	removerDTBDeCola(nuevoDTB->id);
	nuevoDTB->estado = nuevoEstado;

	logguearCambioEstado(nuevoDTB, nuevoEstado);
	waitMutex(&mutexListaDTBs);
	list_add(listaDeTodosLosDTBs, nuevoDTB);
	signalMutex(&mutexListaDTBs);
}

void logguearCambioEstado(DTB* dtb, char nuevoEstado){
		log_info(logger,
				"Pasado a %s dtb con id: %d, escriptorio: %s, quantum: %d",
				 traducirEstado(nuevoEstado), dtb->id, dtb->escriptorio, dtb->quantum);
}


