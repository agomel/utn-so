#include "hilos.h"

#include <pthread.h>


pthread_t crearHilo(void* funcion,void* parametro){
	pthread_t hiloConsola;
	pthread_create(&hiloConsola, NULL, funcion, parametro);
	return hiloConsola;
}

