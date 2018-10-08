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

void entenderMensaje(int emisor, char header){
	char identificado;
	int idDTB;
	DTB* dtb;
	t_dictionary* direccionesYArchivos;
	t_list* lista;
	char* path;
	switch(header){
		case IDENTIFICARSE:
			identificado = deserializarChar(emisor);
			printf("identificado %c \n", identificado);
			switch(identificado){
				case CPU:
					socketCPU = emisor;
					conectadoCPU = 1;
					break;
				case DAM:
					socketDAM = emisor;
					conectadoDAM = 1;
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
			pasarDTBAExit(idDTB);
			break;

		case OK_CARGA_DEL_SCRIPTORIO:
			idDTB = deserializarInt(emisor);
			path = deserializarString(emisor);
			t_list* listaDirecciones = deserializarListaInt(emisor);
			dtb = obtenerDTBDeCola(idDTB);
			dictionary_put(dtb->direccionesArchivos, path, listaDirecciones);
			ponerEnReady(dtb->id);
			break;

		case DESBLOQUEAR_DTB:
			*dtb = deserializarDTB(emisor);
			desbloquearDTB(dtb);
			break;

		case BLOQUEAR_DTB:
			*dtb = deserializarDTB(emisor);
			//TODO cambiar quantum
			cambiarEstado(dtb->id, BLOCKED);
			break;

		case PASAR_A_EXIT:
			*dtb = deserializarDTB(emisor);
			cambiarEstado(dtb->id, EXIT);
			signalSem(&gradoMultiprocesamiento);
			break;

		case TERMINO_QUANTUM:
			*dtb = deserializarDTB(emisor);
			cambiarEstado(dtb->id, READY);
			signalSem(&gradoMultiprocesamiento);
			signalSem(&cantidadTotalREADY);
			break;

		case ERROR:
			idDTB = deserializarInt(emisor);
			path = deserializarString(emisor);
			int error = deserializarInt(emisor);
			manejarErrores(idDTB, path, error);
			break;

		default:
			perror("Cualquiera ese header flaco");
	}
}

void inicializarSAFA(){
	estado = CORRUPTO;
	contadorIds = 1;
	conectadoCPU = 0;
	conectadoDAM = 0;

}
int main(void) {
	inicializarSAFA();

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


	while(!conectadoCPU || !conectadoDAM);
	estado = OPERATIVO;

	esperarHilo(hiloAdministradorDeConexiones);
	esperarHilo(hiloConsola);
	esperarHilo(hiloPlanificadorALargoPlazo);
	esperarHilo(hiloPlanificadorACortoPlazo);
	return 0;
}
