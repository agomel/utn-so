/*
 ============================================================================
 Name        : S-AFA.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <malloc.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include "socket.h"
#include <pthread.h>
#include <unistd.h>
#include "hilos.h"


void consola(){
	while(1){
		printf("Soy la brencha \n");
		sleep(1);
	}
}

int main(void) {

	pthread_t hiloConsola=crearHilo(&consola,NULL);

	int servidor = crearServidor(20000, INADDR_ANY, 100);

	int cliente = conectarConCliente(servidor);
	printf("Recibi una conexion en %d!!\n", cliente);
	send(cliente, "Escribite algo\n", 14, 0);

	char* buffer = malloc(5);

	if(buffer == NULL){
		printf("No hay espacio");
		return 1;
	}

	while(1){
		int bytesRecibidos = recibirMensaje(cliente, &buffer, 4);
		printf("Me llegaron %d bytes con %s \n", bytesRecibidos, buffer);
	}

	free(buffer);

	pthread_join(hiloConsola, NULL);
	return 0;
}
