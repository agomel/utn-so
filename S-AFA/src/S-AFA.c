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

t_dictionary* conexiones;

void entenderMensaje(int emisor, int header){
	char* parametros[1];

	switch(header){
		case identificarse:
			deserializarIdentificarse(emisor,conexiones);
			break;
		case mandarTexto:
			printf("reenviar texto \n");
			deserializar(parametros,emisor);
			int socketCpu=dictionary_get(conexiones,"CPU");
			printf("reenviar mensaje: %s \n",parametros[0]);
			char* mensajeSerializado=serializarMensaje(mandarTexto,parametros[0]);
			enviarMensaje(socketCpu,parametros[0]);
			free(mensajeSerializado);
			break;
		default:
			perror("Cualquiera ese header flaco");

	}

}

int main(void) {
	conexiones = dictionary_create();
	t_config* configuracion = config_create(ARCHIVO_CONFIGURACION);
	int puertoSAFA = config_get_int_value(configuracion, "PUERTO");
	int servidor = crearServidor(puertoSAFA, INADDR_ANY);

	parametrosEscucharClientes parametros;
	parametros.servidor = servidor;
	parametros.funcion = &entenderMensaje;
	pthread_t hiloAdministradorDeConexiones = crearHilo(&escucharClientes, &parametros);
	pthread_t hiloConsola = crearHilo(&consola, NULL);

	esperarHilo(hiloAdministradorDeConexiones);
	esperarHilo(hiloConsola);

	return 0;
}
