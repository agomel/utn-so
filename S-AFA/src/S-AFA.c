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

int socketCPU;
int socketDAM;

void entenderMensaje(int emisor, char header){
	char identificado;
	switch(header){
		case IDENTIFICARSE:
			//TODO agregar tambien el socket identificado al mapa de conexiones
			identificado = deserializarIdentificarse(emisor);
			printf("identificado %c \n" , identificado);
			switch(identificado){
				case CPU:
					socketCPU = emisor;
					break;
				case DAM:
					socketDAM = emisor;
					break;
				default:
					perror("no acepto a esta conexion");

			}
			printf("Se agrego a las conexiones %c \n" , identificado);

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
	direccionServidor direccionSAFA = levantarDeConfiguracion(NULL, "PUERTO", ARCHIVO_CONFIGURACION);
	int servidor = crearServidor(direccionSAFA.puerto, INADDR_ANY);


	pthread_t hiloAdministradorDeConexiones = crearHilo(&escucharClientes, servidor);
	pthread_t hiloConsola = crearHilo(&consola, NULL);

	esperarHilo(hiloAdministradorDeConexiones);
	esperarHilo(hiloConsola);

	return 0;
}
