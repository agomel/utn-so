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
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <malloc.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include "socket.h"
#include <pthread.h>
#include <unistd.h>
#include "hilos.h"
#include "utilidades.h"


void consola(){
	while(1){
		char mensaje[1000];
		scanf("%s", mensaje);
		if(strcmp(mensaje, "exit") == 0){
			exit(1);
		}else if(strcmp(mensaje, "ejecutar") == 0){
			printf("Reconocer ejecutar...\n");
		}else if(strcmp(mensaje, "status") == 0){
			printf("Reconocer status...\n");
		}else if(strcmp(mensaje, "finalizar") == 0){
			printf("Reconocer finalizar...\n");
		}else if(strcmp(mensaje, "metricas") == 0){
			printf("Reconocer metricas...\n");
		}else{
			printf("No te entiendo.\n");
		}
	}
}

int main(void) {

	pthread_t hiloConsola = crearHilo(&consola,NULL);

	int servidor = crearServidor(20000, INADDR_ANY, 100);

	int cliente = conectarConCliente(servidor);
	printf("Recibi una conexion en %d!!\n", cliente);
	enviarMensaje(cliente, "Escribite algo\n");

	char* buffer=asignarMemoria(5);

	int a = 1;
	while(a){
		int bytesRecibidos = recibirMensaje(cliente, &buffer, 4);
		if(bytesRecibidos <= 0){
				perror("Se desconectó el cliente.");
				a=0;
		}else{
			printf("Me llegaron %d bytes con %s \n", bytesRecibidos, buffer);
		}
	}

	free(buffer);

	esperarHilo(hiloConsola);

	return 0;
}
