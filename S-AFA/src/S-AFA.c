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

int main(void) {
	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = INADDR_ANY;
	direccionServidor.sin_port = htons(20000);

	int servidor = socket(AF_INET, SOCK_STREAM, 0);

	int activado = 1;
	setsockopt(servidor, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));

	if(bind(servidor, (void*) &direccionServidor, sizeof(direccionServidor)) != 0){
		perror("FALLÓ EL BIND");
		return 1;
	}

	puts("Estoy escuchando");
	listen(servidor, 100);

	//-----------------------------

	struct sockaddr_in direccionCliente;
	unsigned int tamanioDireccion;
	int cliente = accept(servidor, (void*) &direccionCliente, &tamanioDireccion);

	printf("Recibi una conexion en %d!!\n", cliente);
	send(cliente, "Escribi algo\n", 14, 0);

	//----------------------------

	int a;
	char* buffer = malloc(5);

	if(buffer == NULL){
		printf("No hay espacio");
	}

	while(1){
		int bytesRecibidos = recv(cliente, buffer, 4, 0);
		if(bytesRecibidos <= 0){
			perror("Se desconectó el cliente.");
			return 1;
		}
		buffer[bytesRecibidos] = '\0';
		printf("Me llegaron %d bytes con %s\n", bytesRecibidos, buffer);
	}
	free(buffer);

	return 0;
}
