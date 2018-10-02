#include "planificador.h"
#include "S-AFA.h"

void inicializarPlanificadores(){
	inicializarColas();
	inicializarSemaforos();
}

void inicializarColas(){
	colaNEW = queue_create();
	colaREADY = list_create();
	colaEsperandoDummy = list_create();
	colaEXECUTE = queue_create();
	colaBLOCKED = queue_create();
}

void inicializarSemaforos(){
	inicializarMutex(&mutexNEW);
	inicializarMutex(&mutexREADY);

	u_int32_t multiprogramacion = config_get_int_value(archivoConfiguracion, "MULTIPROGRAMACION");
	inicializarSem(&gradoMultiprogramacion, multiprogramacion);
	inicializarSem(&cantidadTotalREADY, 0);
}
