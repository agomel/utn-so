#include <stdlib.h>
#include <stdio.h>
#include "socket.h"


int crearServidor(int puerto, char* ip, int cantidadAEscuchar){
	struct sockaddr_in direccionServidor = crearDireccionServidor(puerto, ip);

	int servidor = crearSocket();

	int activado = 1;
	//eliminar error de puerto ocupado
	setsockopt(servidor, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));

	if(bind(servidor, (void*) &direccionServidor, sizeof(direccionServidor)) != 0){
		perror("FALLÓ EL BIND");
		exit(1);
	}
	empezarAEscuchar(servidor, cantidadAEscuchar);
	return servidor;
}

static struct sockaddr_in crearDireccionServidor(int puerto, char* ip){
	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = ip;
	direccionServidor.sin_port = htons(puerto);
	return direccionServidor;
}

static int crearSocket(){
	int newSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(newSocket == -1){
			perror("FALLÓ LA CREACION DEL SOCKET");
			exit(1);
	}
	return newSocket;
}

static void empezarAEscuchar(int servidor, int cantidadAEscuchar){
	listen(servidor, cantidadAEscuchar);
	puts("Estoy escuchando");
}


int aceptarCliente(int servidor){
	struct sockaddr_in direccionCliente;
	int tamanioDireccion = sizeof(struct sockaddr_in);
	return accept(servidor, (void*) &direccionCliente, &tamanioDireccion);
}

int recibirMensaje(int socketEmisor, char** buffer, int bytesMaximos){
	int bytesRecibidos = recv(socketEmisor, *buffer, bytesMaximos, 0);
	(*buffer)[bytesRecibidos] = '\0';
	return bytesRecibidos;
}

int conectarConServidor(int puerto, char* ip){
	struct sockaddr_in direccionServidor = crearDireccionServidor(puerto, ip);
	int socketCliente = crearSocket();
	if(connect(socketCliente, (void*) &direccionServidor, sizeof(direccionServidor)) != 0){
		perror("No se pudo conectar");
		exit(1);
	}
	return socketCliente;
}

void enviarMensaje(int socket, char* mensaje){
	send(socket, mensaje, strlen(mensaje), 0);
}




