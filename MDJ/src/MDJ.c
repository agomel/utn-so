/*
 ============================================================================
 Name        : MDJ.c
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
#include "operaciones.h"
#include <commons/collections/queue.h>
#include <biblioteca/semaforos.h>

int socketDAM;


t_queue* colaOperaciones;
pthread_mutex_t mutexOperaciones;
sem_t semOperaciones;


void agregarPedidoACola(char header,int socket){
	OperacionSocket* operacion = asignarMemoria(sizeof(OperacionSocket));
	operacion->header = header;
	operacion->socket = socket;
	waitMutex(&mutexOperaciones);
	queue_push(colaOperaciones, operacion);
	signalMutex(&mutexOperaciones);

}

void escucharCliente(int socket){
	log_debug(logger, "Escuchando a cpu");
	while(1){
		char header;
		recibirMensaje(socket, &header, sizeof(char));
		agregarPedidoACola(header, socket);
		entenderMensaje(socket, header);
		signalSem(&semOperaciones);
		//esto solo agrega operaciones a la cola
	}
}

void consumirCola(){
	while(1){
		waitSem(&semOperaciones);
		OperacionSocket* operacion = queue_pop(colaOperaciones);
		entenderMensaje(operacion->socket, operacion->header);
	}

}


void entenderMensaje(int emisor, char header){
	char identificado;
	int archivoValido;
	char* datos;
	switch(header){
		case IDENTIFICARSE:
			identificado = deserializarChar(emisor);
			log_debug(logger, "Handshake de: %c", identificado);
			switch(identificado){
				case DAM:
					socketDAM = emisor;
					break;
				default:
					log_error(logger, "Conexion rechazada");
			}
			log_debug(logger, "Se agrego a las conexiones %c" , identificado);
			break;
			case VALIDAR_ARCHIVO:
				archivoValido = validarArchivo(emisor);
				enviarYSerializarIntSinHeader(emisor, archivoValido);
				break;
			case CREAR_ARCHIVO:
				crearArchivo(emisor);
				break;
			case OBTENER_DATOS:
				datos = obtenerDatos(emisor);
				enviarYSerializarStringSinHeader(emisor, datos);
				break;
			case GUARDAR_DATOS:
				guardarDatos(emisor);
				break;
		default:
			log_error(logger, "Header desconocido");
	}
}
void init(){
	inicializarMutex(&mutexOperaciones);
	colaOperaciones = queue_create();
	inicializarSem(&semOperaciones, 0);
}
int main(void) {
	init();
	logger = crearLogger(ARCHIVO_LOG, "MDJ");

	direccionServidor direccionMDJ = levantarDeConfiguracion(NULL, "PUERTO", ARCHIVO_CONFIGURACION);
	int servidor = crearServidor(direccionMDJ.puerto, INADDR_ANY);

	parametrosEscucharClientes parametros;
	parametros.servidor = servidor;
	parametros.funcion = &entenderMensaje;
	parametros.logger = logger;

	//pthread_t hiloAdministradorDeConexiones = crearHilo(&escucharClientes, &parametros);

	//esperarHilo(hiloAdministradorDeConexiones);


	empezarAEscuchar(servidor, INADDR_ANY);
	int socket = aceptarCliente(servidor);
	crearHiloQueMuereSolo(&escucharCliente, socket);

	pthread_t hiloEscuchador = crearHilo(&consumirCola, NULL);
	esperarHilo(hiloEscuchador);
	return 0;
}
