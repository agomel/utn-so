/*
 ============================================================================
 Name        : S-AFA.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <biblioteca/socket.h>
#include <biblioteca/hilos.h>
#include <biblioteca/utilidades.h>
#include "consola.h"
#include <biblioteca/select.h>

int escucharClientes(int servidor){
	// escuchar
	empezarAEscuchar(servidor,100);
	recibirConexionesYMensajes(servidor);
}

int main(void) {

	pthread_t hiloConsola = crearHilo(&consola,NULL);
	int servidor = crearServidor(20001, INADDR_ANY);

	pthread_t hiloAdministradorDeConexiones = crearHilo(&escucharClientes,servidor);

	/*

	int cliente = aceptarCliente(servidor);
	printf("Recibi una conexion en %d!!\n", cliente);
	enviarMensaje(cliente, "Escribite algo\n");

	char* buffer =asignarMemoria(5);

	int escuchar = 1;
	while(escuchar){
		int bytesRecibidos = recibirMensaje(cliente, &buffer, 4);
		if(bytesRecibidos <= 0){
				perror("Se desconectó el cliente.");
				escuchar=0;
		}else{
			printf("Me llegaron %d bytes con %s \n", bytesRecibidos, buffer);
		}
	}

	free(buffer);*/

	esperarHilo(hiloConsola);

	esperarHilo(hiloAdministradorDeConexiones);

	return 0;
}
