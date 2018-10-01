#include "planificador.h"

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

	//TODO pasarle el espacio disponible en READY (grado de multiprogramacion) por parametro
	inicializarSem(&gradoMultiprogramacion, 3);
	inicializarSem(&cantidadTotalREADY, 0);
}
