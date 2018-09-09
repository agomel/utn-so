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

	esperarHilo(hiloConsola);

	esperarHilo(hiloAdministradorDeConexiones);

	return 0;
}
