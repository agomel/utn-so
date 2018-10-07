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
	t_list* tablaDeDIreccionesYArchivos;
	DTB* dtb;
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

		case FALLO_LA_CARGA_DEL_SCRIPTORIO:
			idDTB = deserializarInt(emisor);
			waitMutex(&mutexColaDummy);
			pasarDTBAExit(idDTB, colaEsperandoDummy);
			signal(&mutexColaDummy);
			break;

		case DESBLOQUEAR_DTB:
			DTB* dtb = deserializarDTB(emisor);
			desbloquearDTB(dtb);
			break;
		case BLOQUEAR_DTB:
			//TODO bloquear DTB
			break;

		case PASAR_READY:
			//TODO deserializar estructura que envia dam
			colaOrigen = deserializarChar(emisor);
			idDTB = deserializarInt(emisor);
			switch(colaOrigen){
				case COLA_NEW:
					tablaDeDIreccionesYArchivos = deserializarListaInt(emisor);
					dtb = obtenerProcesoDeCola(colaEsperandoDummy, idDTB);
					dtb->tablaDireccionesArchivos = tablaDeDIreccionesYArchivos;
					//es el dummy que avisa que el proceso esta listo
					ponerEnReadyProcesoDummyOk(dtb);
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

	direccionServidor direccionSAFA = levantarDeConfiguracion(NULL, "PUERTO", ARCHIVO_CONFIGURACION);
	int servidor = crearServidor(direccionSAFA.puerto, INADDR_ANY);
	inicializarPlanificadores();
	parametrosEscucharClientes parametros;
	parametros.servidor = servidor;
	parametros.funcion = &entenderMensaje;

	pthread_t hiloAdministradorDeConexiones = crearHilo(&escucharClientes, &parametros);
	pthread_t hiloConsola = crearHilo(&consola, NULL);

	pthread_t hiloPlanificadorALargoPlazo = crearHilo(&planificadorALargoPlazo, NULL);
	pthread_t hiloPlanificadorACortoPlazo = crearHilo(&planificadorACortoPlazo, NULL);

	esperarHilo(hiloAdministradorDeConexiones);
	esperarHilo(hiloConsola);
	esperarHilo(hiloPlanificadorALargoPlazo);
	esperarHilo(hiloPlanificadorACortoPlazo);
	return 0;
}
