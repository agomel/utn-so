/*
 ============================================================================
 Name        : FM9.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <biblioteca/utilidades.h>
#include <biblioteca/socket.h>
#include <biblioteca/select.h>
#include <biblioteca/hilos.h>

void entenderMensaje(char* bytesRecibidos){
	printf("bytes recibidos: %s \n",bytesRecibidos);
}
int escucharClientes(int servidor) {
	empezarAEscuchar(servidor, 100);
	recibirConexionesYMensajes(servidor,&entenderMensaje);
}

int main(void) {
	t_config* configuracion = config_create(ARCHIVO_CONFIGURACION);
	int puertoFM9 = config_get_int_value(configuracion, "PUERTO");
	int servidor = crearServidor(puertoFM9, INADDR_ANY);

	pthread_t hiloAdministradorDeConexiones = crearHilo(&escucharClientes,servidor);

	esperarHilo(hiloAdministradorDeConexiones);
}
