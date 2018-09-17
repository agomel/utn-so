#include "planificador.h"

void inicializarColas(){
	colaNEW = queue_create();
	colaREADY = queue_create();
	colaEXECUTE = queue_create();
	colaBLOCKED = queue_create();
}

void inicializarSemaforos(){
	inicializarMutex(mutexNEW);
	inicializarMutex(mutexREADY);

	//TODO pasarle el espacio disponible en READY (grado de multiprogramacion) por parametro
	inicializarSem(espacioDisponibleREADY, 3);
	inicializarSem(cantidadTotalREADY, 0);
}
