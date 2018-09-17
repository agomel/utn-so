#include "planificador.h"

void inicializarColas(){
	colaNEW = queue_create();
	colaREADY = queue_create();
	colaEXECUTE = queue_create();
	colaBLOCKED = queue_create();
}
