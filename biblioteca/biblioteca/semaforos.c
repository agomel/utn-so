#include "semaforos.h"

//MUTEX

void inicializarMutex(pthread_mutex_t* semaforo){
	pthread_mutex_init(semaforo, NULL);
}

int waitMutex(pthread_mutex_t* semaforo){
	return pthread_mutex_lock(semaforo);
}

int signalMutex(pthread_mutex_t* semaforo){
	return pthread_mutex_unlock(semaforo);
}

/////////////////

//CONTADOR

void inicializarSem(sem_t* semaforo, int valorInicial){
	sem_init(semaforo, 0, valorInicial);
}

int waitSem(sem_t* semaforo){
	int numero =  sem_wait(semaforo);
	return numero;
}

int signalSem(sem_t* semaforo){
	return sem_post(semaforo);
}

//////////////
