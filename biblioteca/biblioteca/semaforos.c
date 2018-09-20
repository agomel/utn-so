#include "semaforos.h"

//MUTEX

void inicializarMutex(pthread_mutex_t* semaforo){
	pthread_mutex_init(semaforo, NULL);
}

u_int32_t waitMutex(pthread_mutex_t* semaforo){
	return pthread_mutex_lock(semaforo);
}

u_int32_t signalMutex(pthread_mutex_t* semaforo){
	return pthread_mutex_unlock(semaforo);
}

/////////////////

//CONTADOR

void inicializarSem(sem_t* semaforo, u_int32_t valorInicial){
	sem_init(semaforo, 0, valorInicial);
}

u_int32_t waitSem(sem_t* semaforo){
	int numero =  sem_wait(semaforo);
	return numero;
}

u_int32_t signalSem(sem_t* semaforo){
	return sem_post(semaforo);
}

//////////////
