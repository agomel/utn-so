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


u_int32_t socketDAM;

void entenderMensaje(int emisor, char header){
	char identificado;
	u_int32_t archivoValido;
	char* datos;
	switch(header){
		case IDENTIFICARSE:
			//TODO agregar tambien el socket identificado al mapa de conexiones
			identificado = deserializarChar(emisor);
			printf("identificado %c \n", identificado);
			switch(identificado){
				case DAM:
					socketDAM = emisor;
					break;
				default:
					perror("no acepto a esta conexion");
			}
			printf("Se agrego a las conexiones %c \n" , identificado);
			break;
			case VALIDAR_ARCHIVO:
				archivoValido = validarArchivo(emisor);
				enviarYSerializarInt(emisor, archivoValido, VALIDAR_ARCHIVO);
				break;
			case CREAR_ARCHIVO:
				crearArchivo(emisor);
				break;
			case OBTENER_DATOS:
				datos = obtenerDatos(emisor);
				//TODO corroborar que es esto lo que queremos devolver
				enviarYSerializarString(emisor, datos, DATOS_CONSEGUIDOS);
				break;
			case GUARDAR_DATOS:
				guardarDatos(emisor);
				break;
		default:
			perror("Cualquiera ese header flaco");
	}
}

int main(void) {
	direccionServidor direccionMDJ = levantarDeConfiguracion(NULL, "PUERTO", ARCHIVO_CONFIGURACION);
	int servidor = crearServidor(direccionMDJ.puerto, INADDR_ANY);

	parametrosEscucharClientes parametros;
	parametros.servidor = servidor;
	parametros.funcion = &entenderMensaje;

	pthread_t hiloAdministradorDeConexiones = crearHilo(&escucharClientes, &parametros);

	esperarHilo(hiloAdministradorDeConexiones);

	return 0;
}
