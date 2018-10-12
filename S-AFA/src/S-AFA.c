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

void agregarPedidoACola(char header,int socket){
	OperacionSocket* operacion = asignarMemoria(sizeof(OperacionSocket));
	operacion->header = header;
	operacion->socket = socket;
	waitMutex(&mutexOperaciones);
	queue_push(colaOperaciones, operacion);
	signalMutex(&mutexOperaciones);

}

void escucharCliente(int socket){
	log_debug(logger, "Escuchando a cpu");
	while(1){
		char header;
		recibirMensaje(socket, &header, sizeof(char));
		agregarPedidoACola(header, socket);
		entenderMensaje(socket, header);
		signalSem(&semOperaciones);
		//esto solo agrega operaciones a la cola
	}
}

void consumirCola(){
	while(1){
		waitSem(&semOperaciones);
		OperacionSocket* operacion = queue_pop(colaOperaciones);
		entenderMensaje(operacion->socket, operacion->header);
	}

}

void entenderMensaje(int emisor, char header){
	char identificado;
	int idDTB;
	DTB* dtb;
	dtb->id = -1;
	t_dictionary* direccionesYArchivos;
	t_list* lista;
	char* path;
	switch(header){
		case IDENTIFICARSE:
			identificado = deserializarChar(emisor);
			log_debug(logger, "Handshake de: %c", identificado);
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
			log_debug(logger, "Se agrego a las conexiones %c" , identificado);
			break;

		case MANDAR_TEXTO:
			//TODO esta operacion es basura, es para probar a serializacion y deserializacion
			deserializarString(emisor);
			break;

		case GUARDADO_CON_EXITO:
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
			cambiarEstadoGuardandoNuevoDTB(dtb, BLOCKED);
			break;

		case PASAR_A_EXIT:
			*dtb = deserializarDTB(emisor);
			cambiarEstadoGuardandoNuevoDTB(dtb, EXIT);
			signalSem(&gradoMultiprocesamiento);
			break;

		case TERMINO_QUANTUM:
			*dtb = deserializarDTB(emisor);
			cambiarEstadoGuardandoNuevoDTB(dtb, READY);
			signalSem(&gradoMultiprocesamiento);
			signalSem(&cantidadTotalREADY);
			break;

		case ERROR:
			idDTB = deserializarInt(emisor);
			path = deserializarString(emisor);
			int error = deserializarInt(emisor);
			//manejarErrores(idDTB, path, error);
			break;

		default:
			log_error(logger, "Header desconocido");
	}
	verificarSiPasarAExit(emisor,dtb);
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
}
int main(void) {
	inicializarSAFA();
	direccionServidor direccionSAFA = levantarDeConfiguracion(NULL, "PUERTO", ARCHIVO_CONFIGURACION);
	int servidor = crearServidor(direccionSAFA.puerto, INADDR_ANY);
	inicializarPlanificadores();
	parametrosEscucharClientes parametros;
	parametros.servidor = servidor;
	parametros.funcion = &entenderMensaje;
	parametros.logger = logger;
	empezarAEscuchar(servidor, INADDR_ANY);
	for(int i = 0; i<2; i++){
		int socket = aceptarCliente(servidor);
		crearHiloQueMuereSolo(&escucharCliente, socket);
	}
	pthread_t hiloConsola = crearHilo(&consola, NULL);
	pthread_t hiloPlanificadorALargoPlazo = crearHilo(&planificadorALargoPlazo, NULL);
	pthread_t hiloPlanificadorACortoPlazo = crearHilo(&planificadorACortoPlazo, NULL);

	pthread_t hiloEscuchador = crearHilo(&consumirCola, NULL);
	while(!conectadoCPU || !conectadoDAM);
	estado = OPERATIVO;

	esperarHilo(hiloEscuchador);
	esperarHilo(hiloConsola);
	esperarHilo(hiloPlanificadorALargoPlazo);
	esperarHilo(hiloPlanificadorACortoPlazo);
	return 0;
}
