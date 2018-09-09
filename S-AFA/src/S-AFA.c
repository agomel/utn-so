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
#include "select.h"

int main(void) {

	pthread_t hiloConsola = crearHilo(&consola,NULL);
	pthread_t hiloAdministradorDeConexiones = crearHilo(&administradorDeConexiones,NULL);

	/*int servidor = crearServidor(20001, INADDR_ANY, 100);

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
