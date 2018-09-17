#ifndef SEMAFOROS_H_
#define SEMAFOROS_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>

//MUTEX

pthread_mutex_t crearMutex();

lock(pthread_mutex_t* semaforo);

unlock(pthread_mutex_t* semaforo);

/////////////////

//CONTADOR

sem_t crearContador(u_int32_t valorInicial);

wait(sem_t* semaforo);

signal(sem_t* semaforo);

//////////////


#endif /*SEMAFOROS_H_*/
