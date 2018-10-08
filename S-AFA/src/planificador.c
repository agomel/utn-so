#include "planificador.h"
#include "S-AFA.h"

void inicializarPlanificadores(){
	configuracion = config_create(ARCHIVO_CONFIGURACION);
	inicializarColas();
	inicializarSemaforos();
	dtbDummy = asignarMemoria(sizeof(DTB));
}

void inicializarColas(){
	colaNEW = queue_create();
	colaREADY = list_create();
	colaEsperandoDummy = list_create();
	colaEXECUTE = list_create();
	colaBLOCKED = list_create();
	colaEXIT = list_create();
	listaDeTodosLosDTBs = list_create();
}

void inicializarSemaforos(){
	inicializarMutex(&mutexNEW);
	inicializarMutex(&mutexREADY);
	inicializarMutex(&mutexEXECUTE);
	inicializarMutex(&mutexBLOCKED);
	inicializarMutex(&mutexEXIT);
	inicializarMutex(&mutexColaDummy);
	inicializarMutex(&mutexIdsDTB);
	inicializarMutex(&mutexListaDTBs);
	inicializarMutex(&mutexDummy);

	int multiprogramacion = config_get_int_value(configuracion, "MULTIPROGRAMACION");
	inicializarSem(&gradoMultiprogramacion, multiprogramacion);
	inicializarSem(&cantidadTotalREADY, 0);
	inicializarSem(&semCantidadEnNew, 0);
}

t_list* filtrarListaPorEstado(char estado){
	bool estaEnEstado(DTB* dtb){
		return (dtb->estado == estado);
	}
	return list_filter(listaDeTodosLosDTBs, estaEnEstado);
}

DTB* cambiarDTBDeColaBuscandoloEnListaDeTodos(DTB* dtb, t_list* nuevaLista){
	obtenerDTBDeCola(listaDeTodosLosDTBs, dtb->id);
	t_list* listaDTB = obtenerColaSinNew(dtb->estado);
	obtenerDTBDeColaRemoviendolo(listaDTB, dtb->id);
	list_add(nuevaLista, dtb);
}

t_list* obtenerColaSinNew(char estado){
	switch(estado){
		case BLOCKED:
			return colaBLOCKED;
			break;
		case READY:
			return colaREADY;
			break;
		case EXECUTE:
			return colaEXECUTE;
			break;
		case EXIT:
			return colaEXIT;
			break;
		case ESPERANDO_DUMMY:
			return colaEsperandoDummy;
			break;
		default:
			perror("No se encontro el DTB en ninguna cola");
	}
}
