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
#include <biblioteca/nuestroSelect.h>
#include <biblioteca/traductor.h>

int socketDAM;
int RETARDO;

t_queue* colaOperaciones;
pthread_mutex_t mutexOperaciones;
sem_t semOperaciones;
sem_t semProductores;

void entenderMensaje(int emisor, char header){
	char* datos;
	log_info(logger, "aaaaaaaaaaaa");
	usleep(RETARDO*1000);//tiempo en milisegundos
	log_info(logger, "bbbbbbb");
	switch(header){
			case VALIDAR_ARCHIVO:
				log_info(logger, "validando archivo de emisor %d", emisor);
				int archivoValido = validarArchivo(emisor);
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
int identificarse(int emisor, char header){
	if(header == IDENTIFICARSE){
		char identificado = deserializarChar(emisor);
		log_debug(logger, "Handshake de: %s", traducirModulo(identificado));
		switch(identificado){
			case DAM:
				socketDAM = emisor;
				break;
			default:
				log_error(logger, "Conexion rechazada");
		}
		log_debug(logger, "Se agrego a las conexiones %s" , traducirModulo(identificado));
		return 1;
	}else{
		return 0;
	}
}



void crearSelect(int servidor){
	Select* select = asignarMemoria(sizeof(Select));
	select->colaOperaciones = colaOperaciones;
	select->funcionEntenderMensaje = &entenderMensaje;
	select->logger = logger;
	select->mutexOperaciones = &mutexOperaciones;
	select->semOperaciones = &semOperaciones;
	select->socket = servidor;
	select->identificarse = &identificarse;
	select->semProductores = &semProductores;
	realizarNuestroSelect(select);
}

void init(){
	t_config* configuracion = config_create(ARCHIVO_CONFIGURACION);
	RETARDO = config_get_int_value(configuracion, "RETARDO");
	free(configuracion);

	logger = crearLogger(ARCHIVO_LOG, "MDJ");

	inicializarMutex(&mutexOperaciones);
	colaOperaciones = queue_create();
	inicializarSem(&semOperaciones, 0);
	inicializarSem(&semProductores, 0);
}
int main(void) {
	init();

	direccionServidor direccionMDJ = levantarDeConfiguracion(NULL, "PUERTO", ARCHIVO_CONFIGURACION);
	int servidor = crearServidor(direccionMDJ.puerto, INADDR_ANY);
	crearSelect(servidor);
	while(1);
	return 0;
}
