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

void deserializarIdentificarse(int emisor){

	char* parametros[3];
	printf("Esto es el identificador \n");
	deserializar(parametros,emisor);
	for(int i = 0;i<3;i++){
		printf("parametro que me mandaste fue: %s \n",parametros[i]);
		free((parametros)[i]);
	}
}
void entenderMensaje(int emisor, int header){
	printf("servidor: %d header: %d \n", emisor, header);
	char* buffer;
	switch(header){
		case identificarse:
			deserializarIdentificarse(emisor);
			break;
		case mandarTexto:
			printf("Esto es el mandarText");
			buffer = asignarMemoria(1000);
			recibirMensaje(emisor, &buffer, 999);
			printf("El resto del mensaje es: %s \n", buffer);
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
	//t_config* configuracion = config_create(ARCHIVO_CONFIGURACION);
	//int puertoSAFA = config_get_int_value(configuracion, "PUERTO");
	int servidor = crearServidor(20001, INADDR_ANY);

	pthread_t hiloAdministradorDeConexiones = crearHilo(&escucharClientes,servidor);
	pthread_t hiloConsola = crearHilo(&consola, NULL);

	esperarHilo(hiloAdministradorDeConexiones);
	esperarHilo(hiloConsola);

	return 0;
}
