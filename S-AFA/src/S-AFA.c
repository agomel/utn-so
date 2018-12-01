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
		switch(identificado){
			case CPU:{
				SocketCPU* socketCPU = asignarMemoria(sizeof(SocketCPU));
				socketCPU->socket = emisor;
				socketCPU->ocupado = 0;
				waitMutex(&mutexSocketsCPus);
				list_add(socketsCPUs, socketCPU);
				signalMutex(&mutexSocketsCPus);
				signalSem(&gradoMultiprocesamiento);
				signalSem(&semCorrupto);
				break;
			}
			case DAM:
				socketDAM = emisor;
				signalSem(&semCorrupto);
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
	log_debug(logger, "El DTB ejecuto %d sentencias", sentenciasEjecutadas);
	agregarSentencias(sentenciasEjecutadas);
	verificarSiPasarAExit(emisor, dtb);
	liberarCPU(emisor, dtb->id);
}

void despedirACPUs(){
	waitMutex(&mutexSocketsCPus);
	for(int i = 0; i < socketsCPUs->elements_count; i++){
		SocketCPU* socketCPU = list_get(socketsCPUs, i);
		enviarYSerializarCharSinHeader(socketCPU->socket, FINALIZARME);
	}
	signalMutex(&mutexSocketsCPus);
}
void despedirACpusMenos(int emisor){
	waitMutex(&mutexSocketsCPus);
	for(int i = 0; i < socketsCPUs->elements_count; i++){
		SocketCPU* socketCPU = list_get(socketsCPUs, i);
		if(socketCPU->socket != emisor)
		enviarYSerializarCharSinHeader(socketCPU->socket, FINALIZARME);
	}
	signalMutex(&mutexSocketsCPus);
}

void entenderMensaje(int emisor, char header){
	int idDTB;
	t_dictionary* direccionesYArchivos;
	t_list* lista;
	char* path;
	char* recurso;
	char asignado;
	Historial* historial;
	usleep(retardo*100);//milisegundos
	switch(header){
		case CARGADO_CON_EXITO_EN_MEMORIA:{
			log_info(logger, "Recibi cargado con exito en memoria del DAM");
			idDTB = deserializarInt(emisor);
			path = deserializarString(emisor);
			int pesoArchivo = deserializarInt(emisor);
			DTB* dtb = obtenerDTBDeCola(idDTB);

			bool compararPath(char* pathDeLista){
				if(strcmp(pathDeLista, path) == 0){
					return true;
				}else{
					return false;
				}
			}

			if(!list_any_satisfy(dtb->listaDeArchivos, compararPath)){
				list_add(dtb->listaDeArchivos, path);
				dtb->tamanioArchivosAbiertos += pesoArchivo;
			}

			operacionDelDiego(idDTB);
			desbloquearDTBCambiandolo(dtb);
			break;

		}

		case DUMMY:{
			log_info(logger, "Desbloqueo el DUMMY");
			DTB* dtb = deserializarDTB(emisor);
			historial = crearHistorial(dtb->id);
			agregarHistorialAListaTiempoRespuesta(historial);
			enviarYSerializarCharSinHeader(emisor, CONTINUAR_CON_EJECUCION);
			break;
		}

		case DESBLOQUEAR_DTB:{
			log_info(logger, "Recibi Desbloquear DTB");
			DTB* dtb = deserializarDTB(emisor);
			desbloquearDummy();

			terminarOperacionDeCPU(emisor, dtb);
			free(dtb);
			break;
		}

		case GUARDADO_CON_EXITO_EN_MDJ:
		case BORRADO_CON_EXITO_EN_MDJ:
		case CREADO_CON_EXITO_EN_MDJ:
			idDTB = deserializarInt(emisor);
			path = deserializarString(emisor);
			log_info(logger, "Recibi %s con exito en MDJ para idDTB %d y path %s", traducirHeaderExito(header), idDTB, path);
			operacionDelDiego(idDTB);
			DTB* dtb = obtenerDTBDeCola(idDTB);
			if(dtb->estado != EXIT)
			desbloquearDTB(idDTB);

			free(path);
			break;

		case ERROR:
			log_info(logger, "Recibi un error");
			idDTB = deserializarInt(emisor);
			path = deserializarString(emisor);
			operacionDelDiego(idDTB);
			int error = deserializarInt(emisor);
			manejarErrores(idDTB, path, error);
			free(path);
			break;

		case BLOQUEAR_DTB:
			log_info(logger, "Recibi bloquear DTB");
			dtb = deserializarDTB(emisor);
			cambiarEstadoGuardandoNuevoDTBFreeando(dtb, BLOCKED);

			 historial = crearHistorial(dtb->id);
			 agregarHistorialAListaTiempoRespuesta(historial);

			terminarOperacionDeCPU(emisor, dtb);
			enviarYSerializarCharSinHeader(emisor, CONTINUAR_CON_EJECUCION);
			break;

		case PASAR_A_EXIT:
			log_info(logger, "Recibi pasar a EXIT");
			dtb = deserializarDTB(emisor);
			pasarDTBAExitGuardandoNuevoFreeando(dtb);

			terminarOperacionDeCPU(emisor, dtb);
			break;

		case PASAR_A_EXIT_POR_ERROR:
			log_info(logger, "Recibi pasar a EXIT por un error :(");
			dtb = deserializarDTB(emisor);
			error = deserializarInt(emisor);
			manejarErrores(dtb->id, dtb->escriptorio, error);
			terminarOperacionDeCPU(emisor, dtb);
			free(dtb);
			break;

		case TERMINO_QUANTUM:
			log_info(logger, "Recibi termino quantum");
			dtb = deserializarDTB(emisor);
			if(!estaEnExit(dtb->id)){
				if(!strcmp(algoritmo, "VRR") && dtb->quantum != 0){
					cambiarEstadoGuardandoNuevoDTBFreeando(dtb, READY_PRIORIDAD);
					signalSem(&cantidadTotalREADY);
				}else{
					ponerEnReadyDTBFreeando(dtb);
				}
			}

			terminarOperacionDeCPU(emisor, dtb);
			break;

		case LIBERAR_RECURSO:
			log_info(logger, "Recibi liberar recurso");
			recurso = deserializarString(emisor);
			asignado = liberarRecurso(recurso);
			enviarYSerializarCharSinHeader(emisor, asignado);
			break;

		case RETENCION_DE_RECURSO:
			log_info(logger, "Recibi retener recurso");
			recurso = deserializarString(emisor);
			idDTB = deserializarInt(emisor);
			asignado = asignarRecurso(idDTB, recurso);
			enviarYSerializarCharSinHeader(emisor, asignado);
			break;
		case FINALIZARME:
			despedirACPUs();
			exit(1);
			break;

		case CHAUCHI_CPU:
			despedirACpusMenos(emisor);
			enviarYSerializarCharSinHeader(socketDAM, FINALIZARME);
			exit(1);
			break;
		default:
			log_error(logger, "Header desconocido del emisor %d y header %c", emisor, header);
	}
}

void inicializarSAFA(){
	estado = CORRUPTO;
	contadorIds = 1;
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
	inicializarMutex(&mutexRecursos);
	recursos = dictionary_create();
	inicializarMutex(&mutexEsperandoRecursos);
	esperandoRecursos = list_create();
	inicializarSem(&semDummy, 1);
	inicializarMutex(&mutexDummy);
	inicializarSem(&semCorrupto, 0);
}
pthread_t crearSelect(int servidor){
	Select* select = asignarMemoria(sizeof(Select));
	select->colaOperaciones = colaOperaciones;
	select->funcionEntenderMensaje = &entenderMensaje;
	select->logger = logger;
	select->mutexOperaciones = &mutexOperaciones;
	select->semOperaciones = &semOperaciones;
	select->socket = servidor;
	select->identificarse = &identificarse;
	select->semProductores = &semProductores;
	return realizarNuestroSelect(select);
}
void despedida(){
	log_info(logger, "chauuuuu :)");
	enviarYSerializarCharSinHeader(socketDAM, FINALIZARME);
	despedirACPUs();
	raise(SIGTERM);
}
int main(void) {
	signal(SIGINT, despedida);
	inicializarSAFA();
	configuracion = config_create(ARCHIVO_CONFIGURACION);
	retardo = config_get_int_value(configuracion, "RETARDO_PLANIF");

	direccionServidor direccionSAFA = levantarDeConfiguracion(NULL, "PUERTO", configuracion);
	int servidor = crearServidor(direccionSAFA.puerto, INADDR_ANY);

	inicializarPlanificadores();
	pthread_t hiloSelect = crearSelect(servidor);
	pthread_t hiloConsola = crearHilo(&consola, NULL);

	waitSem(&semCorrupto);
	waitSem(&semCorrupto);
	estado = OPERATIVO;

	pthread_t hiloPlanificadorALargoPlazo = crearHilo(&planificadorALargoPlazo, NULL);
	pthread_t hiloPlanificadorACortoPlazo = crearHilo(&planificadorACortoPlazo, NULL);

	esperarHilo(hiloConsola);
	esperarHilo(hiloPlanificadorALargoPlazo);
	esperarHilo(hiloPlanificadorACortoPlazo);
	esperarHilo(hiloSelect);

	config_destroy(configuracion);
	return 0;
}
