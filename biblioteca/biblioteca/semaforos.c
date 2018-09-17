#include "semaforos.h"

//MUTEX

pthread_mutex_t crearMutex(){
	pthread_mutex_t semaforo;
	pthread_mutex_init(&semaforo, NULL);
	return semaforo;
}

u_int32_t lock(pthread_mutex_t* semaforo){
	return pthread_mutex_lock(semaforo);
}

u_int32_t unlock(pthread_mutex_t* semaforo){
	return pthread_mutex_unlock(semaforo);
}

/////////////////

//CONTADOR

sem_t crearContador(u_int32_t valorInicial){
	sem_t semaforo;
	sem_init(&semaforo, 0, valorInicial);
	return semaforo;
}

u_int32_t wait(sem_t* semaforo){
	extern int numero =  sem_wait(semaforo);
	return numero;
}

u_int32_t signal(sem_t* semaforo){
	return sem_post(semaforo);
}

//////////////
