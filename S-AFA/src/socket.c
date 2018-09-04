#include <stdlib.h>
#include <stdio.h>
#include "socket.h"


int crearServidor(int puerto, char* ip, int cantidadAEscuchar){
	struct sockaddr_in direccionServidor = crearDireccionServidor(puerto, ip);

	int servidor = socket(AF_INET, SOCK_STREAM, 0);
	if(servidor == -1){
		perror("FALLÓ LA CREACION DEL SOCKET");
		exit(1);
	}

	int activado = 1;
	setsockopt(servidor, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));

	if(bind(servidor, (void*) &direccionServidor, sizeof(direccionServidor)) != 0){
		perror("FALLÓ EL BIND");
		exit(1);
	}
	empezarAEscuchar(servidor, cantidadAEscuchar);
	return servidor;
}

struct sockaddr_in crearDireccionServidor(int puerto, char* ip){
	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = ip;
	direccionServidor.sin_port = htons(puerto);
	return direccionServidor;
}

void empezarAEscuchar(int servidor, int cantidadAEscuchar){
	listen(servidor, cantidadAEscuchar);
	puts("Estoy escuchando");
}


int conectarConCliente(int servidor){
	struct sockaddr_in direccionCliente;
	unsigned int tamanioDireccion;
	return accept(servidor, (void*) &direccionCliente, &tamanioDireccion);
}

int recibirMensaje(int socketEmisor, char** buffer, int bytesMaximos){
	int bytesRecibidos = recv(socketEmisor, *buffer, bytesMaximos, 0);
	if(bytesRecibidos <= 0){
		perror("Se desconectó el cliente.");
		exit(1);
	}
	(*buffer)[bytesRecibidos] = '\0';
	return bytesRecibidos;
}

