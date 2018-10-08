#ifndef SEMAFOROS_H_
#define SEMAFOROS_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>

//MUTEX

void inicializarMutex(pthread_mutex_t* semaforo);

int waitMutex(pthread_mutex_t* semaforo);

int signalMutex(pthread_mutex_t* semaforo);

/////////////////

//CONTADOR

void inicializarSem(sem_t* semaforo, int valorInicial);

int waitSem(sem_t* semaforo);

int signalSem(sem_t* semaforo);

//////////////


#endif /*SEMAFOROS_H_*/
