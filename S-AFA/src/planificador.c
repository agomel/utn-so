#include "planificador.h"
#include "S-AFA.h"

void inicializarPlanificadores(){
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
	contadorIds = 1;

	t_config* configuracion = config_create(ARCHIVO_CONFIGURACION);

	u_int32_t multiprogramacion = config_get_int_value(configuracion, "MULTIPROGRAMACION");
	inicializarSem(&gradoMultiprogramacion, multiprogramacion);
	inicializarSem(&cantidadTotalREADY, 0);
}

