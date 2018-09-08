#include "hilos.h"


pthread_t crearHilo(void* funcion,void* parametro){
	pthread_t hilo;
	pthread_create(&hilo, NULL, funcion, parametro);
	return hilo;
}

int esperarHilo(pthread_t hilo){
	return pthread_join(hilo, NULL);
}

