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
	DTB* dtb;
	t_dictionary* direccionesYArchivos;
	t_list* lista;
	char* path;
	char* recurso;
	char asignado;
	Historial* historial;
	usleep(retardo*100);//milisegundos
	switch(header){
		case MANDAR_TEXTO:
			//TODO esta operacion es basura, es para probar a serializacion y deserializacion
			deserializarString(emisor);
			break;

		case CARGADO_CON_EXITO_EN_MEMORIA:{
			log_info(logger, "Recibi cargado con exito en memoria del DAM");
			idDTB = deserializarInt(emisor);
			path = deserializarString(emisor);
			int pesoArchivo = deserializarInt(emisor);
			dtb = obtenerDTBDeCola(idDTB);

			bool compararPath(char* pathDeLista){
				if(strcmp(pathDeLista, path) == 0){
					return true;
				}else{
					return false;
				}
			}

			if(!list_any_satisfy(dtb->listaDeArchivos, compararPath)){
				log_info(logger, "Agrego path a la lista de archivos del DTB con peso: %d", pesoArchivo);
				list_add(dtb->listaDeArchivos, path);
				dtb->tamanioArchivosAbiertos += pesoArchivo;
			}

			operacionDelDiego(idDTB);
			desbloquearDTB(idDTB);
			break;

		}

		case DUMMY:
			log_info(logger, "Desbloqueo el DUMMY");
			dtb = deserializarDTB(emisor);
			historial = crearHistorial(dtb->id);
			agregarHistorialAListaTiempoRespuesta(historial);
			enviarYSerializarCharSinHeader(emisor, CONTINUAR_CON_EJECUCION);
			log_info(logger, "Mando a CPU que continue con su ejecucion");
			break;

		case DESBLOQUEAR_DTB:
			log_info(logger, "Recibi Desbloquear DTB");
			dtb = deserializarDTB(emisor);
			desbloquearDummy(dtb);

			terminarOperacionDeCPU(emisor, dtb);
			break;

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
			cambiarEstadoGuardandoNuevoDTB(dtb, BLOCKED);
			liberarCPU(emisor, dtb->id);

			 historial = crearHistorial(dtb->id);
			 agregarHistorialAListaTiempoRespuesta(historial);

			terminarOperacionDeCPU(emisor, dtb);
			enviarYSerializarCharSinHeader(emisor, CONTINUAR_CON_EJECUCION);
			break;

		case PASAR_A_EXIT:
			log_info(logger, "Recibi pasar a EXIT");
			dtb = deserializarDTB(emisor);
			cambiarEstadoGuardandoNuevoDTB(dtb, EXIT);

			terminarOperacionDeCPU(emisor, dtb);
			break;

		case TERMINO_QUANTUM:
			log_info(logger, "Recibi termino quantum");
			dtb = deserializarDTB(emisor);
			if(!estaEnExit(dtb->id)){
				if(!strcmp(algoritmo, "VRR") && dtb->quantum != 0){
					cambiarEstadoGuardandoNuevoDTB(dtb, READY_PRIORIDAD);
				}else{
					cambiarEstadoGuardandoNuevoDTB(dtb, READY);
				}
				signalSem(&cantidadTotalREADY);
			}

			terminarOperacionDeCPU(emisor, dtb);
			break;

		case LIBERAR_RECURSO:
			log_info(logger, "Recibi liberar recurso");
			recurso = deserializarString(emisor);
			asignado = liberarRecurso(idDTB, recurso);
			enviarYSerializarCharSinHeader(emisor, asignado);
			log_info(logger, "Enviando a CPU que continue");
			break;

		case RETENCION_DE_RECURSO:
			log_info(logger, "Recibi retener recurso");
			recurso = deserializarString(emisor);
			idDTB = deserializarInt(emisor);
			asignado = asignarRecurso(idDTB, recurso);
			enviarYSerializarCharSinHeader(emisor, asignado);
			log_info(logger, "Enviando a CPU que continue");
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
	inicializarMutex(&mutexRecursos);
	recursos = dictionary_create();
	inicializarMutex(&mutexEsperandoRecursos);
	esperandoRecursos = list_create();
	inicializarSem(&bloqueadoDummy, 1);
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
