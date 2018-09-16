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
#include "operaciones.h"

t_dictionary* conexiones;
u_int32_t socketCPU;
u_int32_t socketDAM;

void entenderMensaje(int emisor, int header){
	char identificado;
	u_int32_t archivoValido;
		switch(header){
			case IDENTIFICARSE:
				//TODO agregar tambien el socket identificado al mapa de conexiones
				identificado = deserializarIdentificarse(emisor);
				printf("identificado %c \n", identificado);
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
				break;
			case VALIDAR_ARCHIVO:
				archivoValido = validarArchivo(emisor);
				//TODO DEVOLVERLE AL EMISOR archivoValido
				break;
			case CREAR_ARCHIVO:
				crearArchivo(emisor);
				break;
			case OBTENER_DATOS:
				break;
			case GUARDAR_DATOS:
				break;
			default:
				perror("Cualquiera ese header flaco");
		}
}

int main(void) {
	direccionServidor direccionFM9 = levantarDeConfiguracion(NULL, "PUERTO", ARCHIVO_CONFIGURACION);
	int servidor = crearServidor(direccionFM9.puerto, INADDR_ANY);

	parametrosEscucharClientes parametros;
	parametros.servidor = servidor;
	parametros.funcion = &entenderMensaje;
	pthread_t hiloAdministradorDeConexiones = crearHilo(&escucharClientes, &parametros);

	esperarHilo(hiloAdministradorDeConexiones);
	return 0;
}
