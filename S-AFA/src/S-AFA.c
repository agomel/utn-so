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
#include <commons/collections/dictionary.h>

t_dictionary* conexiones;

void entenderMensaje(int emisor, char header){
	char identificado[1];
	switch(header){
		case IDENTIFICARSE:
			//TODO agregar tambien el socket identificado al mapa de conexiones
			identificado[0]=deserializarIdentificarse(emisor);
			dictionary_put(conexiones,identificado,emisor);
			dictionary_get(conexiones,identificado);

			break;
		case MANDAR_TEXTO:
			//TODO esta operacion es basura, es para probar a serializacion y deserializacion
			deserializarString(emisor);
			break;
		default:
			perror("Cualquiera ese header flaco");
	}
}
int escucharClientes(int servidor) {
	empezarAEscuchar(servidor, 100);
	recibirConexionesYMensajes(servidor,&entenderMensaje);
}
int main(void) {
	conexiones = dictionary_create();
	t_config* configuracion = config_create(ARCHIVO_CONFIGURACION);
	int puertoSAFA = config_get_int_value(configuracion, "PUERTO");
	int servidor = crearServidor(puertoSAFA, INADDR_ANY);

	pthread_t hiloAdministradorDeConexiones = crearHilo(&escucharClientes, servidor);
	pthread_t hiloConsola = crearHilo(&consola, NULL);

	esperarHilo(hiloAdministradorDeConexiones);
	esperarHilo(hiloConsola);

	return 0;
}
