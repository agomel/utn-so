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


void entenderMensaje(int emisor, char header){
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
	int programCounter;

		switch(header){

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
			case TRAER_LINEA_ESCRIPTORIO:
				programCounter = deserializarInt(emisor);
				posiciones = deserializarListaInt(emisor);
				char* respuesta = "Respuesta hardcodeada";
				for(int i = 0; i < posiciones->elements_count; i ++){
					waitMutex(&mutexStorage);
					//TODO obtener del storage lo pedido y agregar al string realocandole memoria
					signalMutex(&mutexStorage);
				}

				desplazamiento = 0;
				tamanioBuffer = sizeof(int) + strlen(respuesta) + 1;
				buffer = asignarMemoria(tamanioBuffer);
				concatenarString(buffer, &desplazamiento, respuesta);
				enviarMensaje(socketCPU, buffer, tamanioBuffer);
				break;
			default:
				log_error(logger, "Header desconocido");
		}
}
int identificarse(int emisor, char header){
	if(header == IDENTIFICARSE){
		char identificado = deserializarChar(emisor);
		log_debug(logger, "Handshake de: %s", traducirModulo(identificado));
		switch(identificado){
			case CPU:
				socketCPU = emisor;
				break;
			case DAM:
				socketDAM = emisor;
				break;
			default:
				log_error(logger, "Conexion rechazada");
				break;
		}
		log_debug(logger, "Se agrego a las conexiones %s" , traducirModulo(identificado));
		return 1;
	}else{
		return 0;
	}
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
void init(){
	inicializarMutex(&mutexOffset);
	inicializarMutex(&mutexStorage);
	t_config* configuracion = config_create(ARCHIVO_CONFIGURACION);
	storage = asignarMemoria(config_get_int_value(configuracion, "TAMANIO"));
	//TODO el offset no va a ser siempre 0
	offset = 0;
	logger = crearLogger(ARCHIVO_LOG, "FM9");
	inicializarMutex(&mutexOperaciones);
	colaOperaciones = queue_create();
	inicializarSem(&semOperaciones, 0);
	inicializarSem(&semProductores, 0);
}

int main(void) {
	init();

	t_config* configuracion = config_create(ARCHIVO_CONFIGURACION);
	direccionServidor direccionFM9 = levantarDeConfiguracion(NULL, "PUERTO", configuracion);
	int servidor = crearServidor(direccionFM9.puerto, INADDR_ANY);
	crearSelect(servidor);
	config_destroy(configuracion);

	while(1);

	free(storage);
	queue_destroy(colaOperaciones);
	return 0;
}
