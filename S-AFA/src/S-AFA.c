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
	char* parametros[1];
	u_int32_t tam;
	switch(header){
		case IDENTIFICARSE:
			/*
			 * char* parametros[1];
	printf("Esto es el identificador \n");
	deserializar((void**)parametros,emisor);
	printf("Me mandaron %s \n",parametros[0]);
	dictionary_put(conexiones,parametros[0],emisor);
	printf("Agregado %s a las conexiones \n",parametros[0]);
	*/
			recibirMensaje(emisor, &tam, sizeof(u_int32_t));
			char* buffer = malloc(tam);

			recibirMensaje(emisor, buffer, tam);
			printf("Se identifico a %s \n" , buffer);
			//deserializarIdentificarse(emisor,conexiones);
			break;
		case MANDAR_TEXTO:
			//TODO esto no va  aser asi, hay que hacer la posta
			printf("reenviar texto \n");
			/*deserializar(parametros,emisor);
			int socketCpu=dictionary_get(conexiones,"CPU");
			printf("reenviar mensaje: %s \n",parametros[0]);
			char* mensajeSerializado=serializarMensaje(MANDAR_TEXTO,parametros[0]);
			enviarMensaje(socketCpu,parametros[0]);
			free(mensajeSerializado);*/
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
