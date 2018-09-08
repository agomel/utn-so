#include "hilos.h"



pthread_t crearHilo(void* funcion){
	pthread_t hiloConsola;
	pthread_create(&hiloConsola, NULL, &funcion, 3);
	return hiloConsola;
}

