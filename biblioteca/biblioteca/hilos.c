#include "hilos.h"


pthread_t crearHilo(void* funcion, void* parametro){
	pthread_t hilo;
	pthread_create(&hilo, NULL, funcion, parametro);
	return hilo;
}

void crearHiloQueMuereSolo(void* funcion, void* parametro){
	pthread_attr_t attr;
	pthread_t hilo;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	pthread_create(&hilo, &attr, funcion, parametro);
}

int esperarHilo(pthread_t hilo){
	return pthread_join(hilo, NULL);
}

