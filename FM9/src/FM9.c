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

t_dictionary* conexiones;

void entenderMensaje(int emisor, int header){
	deserializarIdentificarse(emisor,conexiones);
}

int main(void) {
	conexiones = dictionary_create();
	t_config* configuracion = config_create(ARCHIVO_CONFIGURACION);
	int puertoFM9 = config_get_int_value(configuracion, "PUERTO");
	int servidor = crearServidor(puertoFM9, INADDR_ANY);

	parametrosEscucharClientes parametros;
	parametros.servidor = servidor;
	parametros.funcion = &entenderMensaje;
	pthread_t hiloAdministradorDeConexiones = crearHilo(&escucharClientes, &parametros);

	esperarHilo(hiloAdministradorDeConexiones);
}
