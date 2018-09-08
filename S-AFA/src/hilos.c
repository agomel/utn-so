#include "hilos.h"


pthread_t crearHilo(void* funcion,void* parametro){
	pthread_t hiloConsola;
	pthread_create(&hiloConsola, NULL, funcion, parametro);
	return hiloConsola;
}

int esperarHilo(pthread_t hilo){
	return pthread_join(hilo, NULL);
}

