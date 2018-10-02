/*
 ============================================================================
 Name        : S-AFA.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include "S-AFA.h"

u_int32_t socketCPU;
u_int32_t socketDAM;

void entenderMensaje(int emisor, char header){
	char identificado;
	char colaOrigen;
	int idDTB;
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

		case MANDAR_TEXTO:
			//TODO esta operacion es basura, es para probar a serializacion y deserializacion
			deserializarString(emisor);
			break;

		case PASAR_EXIT:
			//TODO deserializar estructura que envia dam
			colaOrigen = deserializarChar(emisor);
			idDTB = deserializarInt(emisor);
			//Debería sacarlo de la lista de los dtbs esperando Dummy y ponerlo en exit
			break;

		case PASAR_READY:
			//TODO deserializar estructura que envia dam
			colaOrigen = deserializarChar(emisor);
			idDTB = deserializarInt(emisor);
			switch(colaOrigen){
				case COLA_NEW:
					//es el dummy que avisa que el proceso esta listo
					ponerEnReadyProcesoDummyOk(idDTB);
					break;
				default:
					break;
			}
			break;
		default:
			perror("Cualquiera ese header flaco");
	}
}

int main(void) {
	//TODO Debe iniciar en estado corrupto

	inicializarMutex(&mutexIdsDTB);
	contadorIds = 1;
	direccionServidor direccionSAFA = levantarDeConfiguracion(NULL, "PUERTO", ARCHIVO_CONFIGURACION, archivoConfiguracion);
	int servidor = crearServidor(direccionSAFA.puerto, INADDR_ANY);

	parametrosEscucharClientes parametros;
	parametros.servidor = servidor;
	parametros.funcion = &entenderMensaje;

	pthread_t hiloAdministradorDeConexiones = crearHilo(&escucharClientes, &parametros);
	pthread_t hiloConsola = crearHilo(&consola, NULL);

	inicializarPlanificadores();
	pthread_t hiloPlanificadorALargoPlazo = crearHilo(&planificadorALargoPlazo, NULL);
	pthread_t hiloPlanificadorACortoPlazo = crearHilo(&planificadorACortoPlazo, NULL);

	esperarHilo(hiloAdministradorDeConexiones);
	esperarHilo(hiloConsola);
	esperarHilo(hiloPlanificadorALargoPlazo);
	esperarHilo(hiloPlanificadorACortoPlazo);
	return 0;
}
