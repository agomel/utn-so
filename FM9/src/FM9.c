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

respuestaDeCargaEnMemoria cargarDatosEnMemoria(char* datos){
	respuestaDeCargaEnMemoria respuesta;
	respuesta.listaDeDirecciones = guardarDatos(datos);
	//TODO pudoGuardarlo()
	if(1){
		respuesta.pudoGuardarlo = 1;
	}else{
		respuesta.pudoGuardarlo = 0;
	}
	log_info(logger, "Guardados datos: %s", datos);
	return respuesta; //pudo guardar. TODO hacer si tuvo un error return 0
}

void entenderMensaje(int emisor, int header){
	char identificado;
	char* datos;
	respuestaDeCargaEnMemoria respuestaDeCarga;
	int id;
	int tamanioBuffer;
	int offset;
	int sizeDeLaRespuesta;
	int desplazamiento;
	int tamanioPath;
	void* buffer;
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

				if(respuestaDeCarga.pudoGuardarlo){
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
}

int main(void) {
	init();

	direccionServidor direccionFM9 = levantarDeConfiguracion(NULL, "PUERTO", ARCHIVO_CONFIGURACION);
	int servidor = crearServidor(direccionFM9.puerto, INADDR_ANY);

	parametrosEscucharClientes parametros;
	parametros.servidor = servidor;
	parametros.funcion = &entenderMensaje;
	parametros.logger = logger;
	pthread_t hiloAdministradorDeConexiones = crearHilo(&escucharClientes, &parametros);

	esperarHilo(hiloAdministradorDeConexiones);
	free(storage);
	return 0;
}
