#include "semaforos.h"

//MUTEX

pthread_mutex_t crearMutex(){
	pthread_mutex_t semaforo;
	pthread_mutex_init(&semaforo, NULL);
	return semaforo;
}

lock(pthread_mutex_t* semaforo){
	pthread_mutex_lock(semaforo);
}

unlock(pthread_mutex_t* semaforo){
	pthread_mutex_unlock(semaforo);
}

/////////////////

//CONTADOR

sem_t crearContador(u_int32_t valorInicial){
	sem_t semaforo;
	sem_init(&semaforo, 0, valorInicial);
	return semaforo;
}

wait(sem_t* semaforo){
	sem_wait(semaforo);
}

signal(sem_t* semaforo){
	sem_post(semaforo);
}

//////////////
