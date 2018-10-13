/*
 ============================================================================
 Name        : FM9.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "FM9.h"
t_list* guardarDatos(char* datos){
	t_list* direccionesGuardadas = list_create();
	waitMutex(&mutexOffset);
	list_add(direccionesGuardadas, offset);
	signalMutex(&mutexOffset);

	waitMutex(&mutexStorage);
	memcpy(storage, datos, strlen(datos) + 1);
	signalMutex(&mutexStorage);

	waitMutex(&mutexOffset);
	offset = offset + strlen(datos) + 1;
	signalMutex(&mutexOffset);
	return direccionesGuardadas;
}

respuestaDeObtencionDeMemoria* obtenerDatosDeMemoria(t_list* posiciones){
	//TODO OBTENER DATOS
	respuestaDeObtencionDeMemoria* respuesta;
	respuesta->cantidadDeLineas = 0;
	t_list* lista = list_create();
	respuesta->datos = lista;
	respuesta->pudoGuardarlo = 0;
	return respuesta;
}

respuestaDeCargaEnMemoria cargarDatosEnMemoria(char* datos){
	respuestaDeCargaEnMemoria respuesta;
	respuesta.listaDeDirecciones = guardarDatos(datos);// TODO
	respuesta.pudoGuardarlo = 0;//0 significa que pudo, si no, numero de error
	log_info(logger, "Guardados datos: %s", datos);
	return respuesta; //pudo guardar. TODO hacer si tuvo un error return 0
}


void agregarPedidoACola(char header, int socket){
	OperacionSocket* operacion = asignarMemoria(sizeof(OperacionSocket));
	operacion->header = header;
	operacion->socket = socket;
	waitMutex(&mutexOperaciones);
	queue_push(colaOperaciones, operacion);
	signalMutex(&mutexOperaciones);

}

void escucharCliente(int socket){
	log_debug(logger, "Escuchando nuevo cliente en %d", socket);
	while(1){
		char header;
		recibirMensaje(socket, &header, sizeof(char));
		agregarPedidoACola(header, socket);
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
	char* datos;
	respuestaDeCargaEnMemoria respuestaDeCarga;
	respuestaDeObtencionDeMemoria* respuestaDeObtener;
	int id;
	int tamanioBuffer;
	int offset;
	int sizeDeLaRespuesta;
	int desplazamiento;
	int tamanioPath;
	void* buffer;
	t_list* posiciones;

		switch(header){
			case IDENTIFICARSE:
				identificado = deserializarChar(emisor);
				log_debug(logger, "Handshake de: %c", identificado);
				switch(identificado){
					case CPU:
						socketCPU = emisor;
						break;
					case DAM:
						socketDAM = emisor;
						break;
					default:
						log_error(logger, "Conexion rechazada");
				}
				log_debug(logger, "Se agrego a las conexiones %c" , identificado);
				break;

			case GUARDAR_DATOS:
				datos = deserializarString(emisor);
				respuestaDeCarga = cargarDatosEnMemoria(datos);

				if(respuestaDeCarga.pudoGuardarlo == 0){
					desplazamiento = 0;
					tamanioBuffer = sizeof(int) + sizeof(int) + sizeof(int)*(respuestaDeCarga.listaDeDirecciones->elements_count);
					buffer = asignarMemoria(tamanioBuffer);
					concatenarInt(buffer, &desplazamiento, respuestaDeCarga.pudoGuardarlo);
					concatenarListaInt(buffer, &desplazamiento, respuestaDeCarga.listaDeDirecciones);
					enviarMensaje(socketDAM, buffer, tamanioBuffer);
					free(buffer);

				}else{
					enviarYSerializarIntSinHeader(socketDAM, respuestaDeCarga.pudoGuardarlo);
				}
				break;
			case OBTENER_DATOS:
				posiciones = deserializarListaInt(emisor);
				respuestaDeObtener = obtenerDatosDeMemoria(posiciones); //TODO

				desplazamiento = 0;
				tamanioBuffer = sizeof(int) + sizeof(int) + sizeof(int) + strlen("hola") + 1;
				buffer = asignarMemoria(tamanioBuffer);
				concatenarInt(buffer, &desplazamiento, 0);
				concatenarInt(buffer, &desplazamiento, 1);
				concatenarString(buffer, &desplazamiento, "hola");
				enviarMensaje(socketDAM, buffer, tamanioBuffer);
				break;
			default:
				log_error(logger, "Header desconocido");
		}
}

void init(){
	inicializarMutex(&mutexOffset);
	inicializarMutex(&mutexStorage);
	//TODO cargar storage
	storage = asignarMemoria(1000);
	offset = 0;
	logger = crearLogger(ARCHIVO_LOG, "FM9");
	inicializarMutex(&mutexOperaciones);
	colaOperaciones = queue_create();
	inicializarSem(&semOperaciones, 0);
}

int main(void) {
	init();

	direccionServidor direccionFM9 = levantarDeConfiguracion(NULL, "PUERTO", ARCHIVO_CONFIGURACION);
	int servidor = crearServidor(direccionFM9.puerto, INADDR_ANY);

	parametrosEscucharClientes parametros;
	parametros.servidor = servidor;
	parametros.funcion = &entenderMensaje;
	parametros.logger = logger;
	pthread_t hiloEscuchador = crearHilo(&consumirCola, NULL);
	empezarAEscuchar(servidor, INADDR_ANY);
	while(1){
		int socket = aceptarCliente(servidor);
		crearHiloQueMuereSolo(&escucharCliente, socket);
	}


	//esperarHilo(hiloAdministradorDeConexiones);
	esperarHilo(hiloEscuchador);
	free(storage);
	return 0;
}
