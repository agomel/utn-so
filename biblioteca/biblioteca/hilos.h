#ifndef HILOS_H_
#define HILOS_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

pthread_t crearHilo(void* funcion,void* parametro);
int esperarHilo(pthread_t hilo);

#endif /*HILOS_H_*/
