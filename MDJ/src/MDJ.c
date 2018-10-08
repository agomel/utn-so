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

int socketDAM;

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

int main(void) {
	logger = crearLogger(ARCHIVO_LOG, "MDJ");

	direccionServidor direccionMDJ = levantarDeConfiguracion(NULL, "PUERTO", ARCHIVO_CONFIGURACION);
	int servidor = crearServidor(direccionMDJ.puerto, INADDR_ANY);

	parametrosEscucharClientes parametros;
	parametros.servidor = servidor;
	parametros.funcion = &entenderMensaje;
	parametros.logger = logger;

	pthread_t hiloAdministradorDeConexiones = crearHilo(&escucharClientes, &parametros);

	esperarHilo(hiloAdministradorDeConexiones);

	return 0;
}
