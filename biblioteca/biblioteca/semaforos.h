#ifndef SEMAFOROS_H_
#define SEMAFOROS_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>

//MUTEX

void inicializarMutex(pthread_mutex_t* semaforo);

u_int32_t waitMutex(pthread_mutex_t* semaforo);

u_int32_t signalMutex(pthread_mutex_t* semaforo);

/////////////////

//CONTADOR

void inicializarSem(sem_t* semaforo, u_int32_t valorInicial);

u_int32_t waitSem(sem_t* semaforo);

u_int32_t signalSem(sem_t* semaforo);

//////////////


#endif /*SEMAFOROS_H_*/
