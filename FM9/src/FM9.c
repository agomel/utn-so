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

t_dictionary* conexiones;
u_int32_t socketCPU;
u_int32_t socketDAM;
char* storage;

u_int32_t cargarDatosEnMemoria(char* datos){
	//TODO devolver una estructura que tenga la lista de tabla de paginas
	printf("guardado en memoria: %s", datos);
	return 1; //pudo guardar. TODO hacer si tuvo un error return 0
}

void entenderMensaje(int emisor, int header){
	char identificado;
	char* datos;
	u_int32_t pudoGuardarDatos;

		switch(header){
			case IDENTIFICARSE:
				identificado = deserializarChar(emisor);
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

			case GUARDAR_DATOS:
				datos = deserializarString(emisor);
				pudoGuardarDatos = cargarDatosEnMemoria(datos);
				//TODO enviar y serializar la estructura de guardado
				enviarYSerializarInt(socketDAM, pudoGuardarDatos, RESPUESTA_CARGA);

				break;

			default:
				perror("Cualquiera ese header flaco");
		}
}

int main(void) {
	//TODO cargar storage
	storage = asignarMemoria(1000);

	direccionServidor direccionFM9 = levantarDeConfiguracion(NULL, "PUERTO", ARCHIVO_CONFIGURACION);
	int servidor = crearServidor(direccionFM9.puerto, INADDR_ANY);

	parametrosEscucharClientes parametros;
	parametros.servidor = servidor;
	parametros.funcion = &entenderMensaje;
	pthread_t hiloAdministradorDeConexiones = crearHilo(&escucharClientes, &parametros);

	esperarHilo(hiloAdministradorDeConexiones);
	free(storage);
	return 0;
}
