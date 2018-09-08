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

int main(void) {
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

	return 0;
}
