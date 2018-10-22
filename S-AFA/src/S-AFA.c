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

int identificarse(int emisor, char header){
	if(header == IDENTIFICARSE){
		char identificado = deserializarChar(emisor);
		log_debug(logger, "Handshake de: %s", traducirModulo(identificado));
		switch(identificado){
			case CPU:
				conectadoCPU = 1;
				SocketCPU* socketCPU = asignarMemoria(sizeof(SocketCPU));
				socketCPU->socket = emisor;
				socketCPU->ocupado = 0;
				waitMutex(&mutexSocketsCPus);
				list_add(socketsCPUs, socketCPU);
				signalMutex(&mutexSocketsCPus);
				signalSem(&gradoMultiprocesamiento);
				break;
			case DAM:
				socketDAM = emisor;
				conectadoDAM = 1;
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
void terminarOperacionDeCPU(int emisor, DTB* dtb){
	int sentenciasEjecutadas = deserializarInt(emisor);
	verificarSiPasarAExit(emisor, dtb);
	liberarCPU(emisor, dtb->id);
}

void entenderMensaje(int emisor, char header){
	int idDTB;
	DTB* dtb;
	t_dictionary* direccionesYArchivos;
	t_list* lista;
	char* path;
	usleep(retardo*100);//milisegundos
	switch(header){
		case MANDAR_TEXTO:
			//TODO esta operacion es basura, es para probar a serializacion y deserializacion
			deserializarString(emisor);
			break;

		case CARGADO_CON_EXITO_EN_MEMORIA:
			//DAM
			idDTB = deserializarInt(emisor);
			path = deserializarString(emisor);
			t_list* listaDirecciones = deserializarListaInt(emisor);
			dtb = obtenerDTBDeCola(idDTB);
			dictionary_put(dtb->direccionesArchivos, path, listaDirecciones);
			ponerEnReady(dtb->id);
			break;

		case DESBLOQUEAR_DTB:
			dtb = deserializarDTB(emisor);
			desbloquearDummy(dtb);

			terminarOperacionDeCPU(emisor, dtb);
			break;

		case GUARDADO_CON_EXITO_EN_MDJ:
			//DAM
			dtb = deserializarDTB(emisor);
			desbloquearDTB(dtb);

			break;
		case ERROR:
			//DAM
			idDTB = deserializarInt(emisor);
			path = deserializarString(emisor);
			int error = deserializarInt(emisor);
			manejarErrores(idDTB, path, error);
			break;

		case BLOQUEAR_DTB:
			dtb = deserializarDTB(emisor);
			//TODO cambiar quantum
			cambiarEstadoGuardandoNuevoDTB(dtb, BLOCKED);

			terminarOperacionDeCPU(emisor, dtb);
			break;

		case PASAR_A_EXIT:
			dtb = deserializarDTB(emisor);
			cambiarEstadoGuardandoNuevoDTB(dtb, EXIT);

			terminarOperacionDeCPU(emisor, dtb);
			break;

		case TERMINO_QUANTUM:
			dtb = deserializarDTB(emisor);
			if(strcmp(algoritmo, "VRR")){
				cambiarEstadoGuardandoNuevoDTB(dtb, READY_PRIORIDAD);
			}else{
				cambiarEstadoGuardandoNuevoDTB(dtb, READY);
			}
			signalSem(&cantidadTotalREADY);

			terminarOperacionDeCPU(emisor, dtb);
			break;
		case LIBERAR_RECURSO:
			//Es de CPU pero no termina
			break;
		case RETENCION_DE_RECURSO:
			//Es de CPU pero no termina
			break;

		default:
			log_error(logger, "Header desconocido");
	}
}

void inicializarSAFA(){
	estado = CORRUPTO;
	contadorIds = 1;
	conectadoCPU = 0;
	conectadoDAM = 0;
	logger = crearLogger(ARCHIVO_LOG, "SAFA");
	socketsCPUs = list_create();
	inicializarMutex(&mutexSocketsCPus);
	ejecutandoCPU = dictionary_create();
	inicializarMutex(&mutexEjecutandoCPU);
	cpusAFinalizarDTBs = dictionary_create();
	inicializarMutex(&mutexCpusAFinalizarDTBs);
	inicializarMutex(&mutexOperaciones);
	colaOperaciones = queue_create();
	inicializarSem(&semOperaciones, 0);
	inicializarSem(&semProductores, 0);
	initGestorDTBs();
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
int main(void) {
	inicializarSAFA();
	t_config* configuracion = config_create(ARCHIVO_CONFIGURACION);
	retardo = config_get_int_value(configuracion, "RETARDO_PLANIF");


	direccionServidor direccionSAFA = levantarDeConfiguracion(NULL, "PUERTO", configuracion);
	int servidor = crearServidor(direccionSAFA.puerto, INADDR_ANY);

	inicializarPlanificadores();
	crearSelect(servidor);
	pthread_t hiloConsola = crearHilo(&consola, NULL);

	while(!conectadoCPU || !conectadoDAM);
	estado = OPERATIVO;

	pthread_t hiloPlanificadorALargoPlazo = crearHilo(&planificadorALargoPlazo, NULL);
	pthread_t hiloPlanificadorACortoPlazo = crearHilo(&planificadorACortoPlazo, NULL);

	esperarHilo(hiloConsola);
	esperarHilo(hiloPlanificadorALargoPlazo);
	esperarHilo(hiloPlanificadorACortoPlazo);

	config_destroy(configuracion);
	return 0;
}
