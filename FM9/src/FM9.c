#include "FM9.h"
#include "segmentacionPura.h"
#include "segmentacionPag.h"
#include "paginasInvertidas.h"

respuestaDeObtencionDeMemoria* obtenerDatosDeMemoria(t_list* posiciones){
	if(strcmp(modo, "SEG_PURA") == 0)
		return obtenerDatosSegPura(posiciones);

	if(strcmp(modo, "SEG_PAG") == 0)
		return obtenerDatosSegPag(posiciones);

	if(strcmp(modo, "INV") == 0)
		return obtenerDatosInvertida(posiciones);
}

respuestaDeCargaEnMemoria cargarDatosEnMemoria(char* datos){
	if(strcmp(modo, "SEG_PURA") == 0)
		return guardarDatosSegPura(datos);

	if(strcmp(modo, "SEG_PAG") == 0)
		return guardarDatosSegPag(datos);

	if(strcmp(modo, "INV") == 0)
		return guardarDatosInvertida(datos);
}

respuestaDeObtencionDeMemoria* obtenerLinea(t_list* ids, int numeroLinea){
	if(strcmp(modo, "SEG_PURA") == 0)
		return obtenerLineaSegPura(ids, numeroLinea);

	if(strcmp(modo, "SEG_PAG") == 0)
		return obtenerLineaSegPura(ids, numeroLinea); //CAMBIAAAAAAAR

	if(strcmp(modo, "INV") == 0)
		return obtenerLineaSegPura(ids, numeroLinea); //CAMBIAAAAAAAAR
}

void freeRespuestaObtencion(respuestaDeObtencionDeMemoria* respuesta){
	free(respuesta->datos);
	free(respuesta);
}

void entenderMensaje(int emisor, char header){
	switch(header){
			case GUARDAR_DATOS: {
				log_debug(logger, "Guardando datos en memoria");
				char* datos = deserializarString(emisor);
				respuestaDeCargaEnMemoria respuestaDeCarga = cargarDatosEnMemoria(datos);

				if(respuestaDeCarga.pudoGuardarlo == 0){
					int desplazamiento = 0;
					int tamanioBuffer = sizeof(int) + sizeof(int) + sizeof(int)*(respuestaDeCarga.listaDeDirecciones->elements_count);
					void* buffer = asignarMemoria(tamanioBuffer);
					concatenarInt(buffer, &desplazamiento, respuestaDeCarga.pudoGuardarlo);
					concatenarListaInt(buffer, &desplazamiento, respuestaDeCarga.listaDeDirecciones);
					enviarMensaje(socketDAM, buffer, tamanioBuffer);
					free(buffer);

				}else{
					enviarYSerializarIntSinHeader(socketDAM, respuestaDeCarga.pudoGuardarlo);
				}
				break;
			}

			case OBTENER_DATOS: {
				log_debug(logger, "Obteniendo datos de memoria");
				t_list* ids = deserializarListaInt(emisor);
				respuestaDeObtencionDeMemoria* respuestaDeObtener = obtenerDatosDeMemoria(ids); //TODO

				int desplazamiento = 0;
				int tamanioBuffer = sizeof(int) + sizeof(int) + sizeof(int) + strlen(respuestaDeObtener->datos) + 1;
				int buffer = asignarMemoria(tamanioBuffer);
				concatenarInt(buffer, &desplazamiento, respuestaDeObtener->pudoObtener);
				concatenarInt(buffer, &desplazamiento, respuestaDeObtener->cantidadDeLineas);
				concatenarString(buffer, &desplazamiento, respuestaDeObtener->datos);
				enviarMensaje(socketDAM, buffer, tamanioBuffer);
				freeRespuestaObtencion(respuestaDeObtener);
				break;
			}

			case TRAER_LINEA_ESCRIPTORIO: {
				log_debug(logger, "Trayendo linea escriptorio");
				int numeroLinea = deserializarInt(emisor);
				t_list* ids = deserializarListaInt(emisor);
				respuestaDeObtencionDeMemoria* respuesta = obtenerLinea(ids, numeroLinea);

				if(respuesta->pudoObtener == 0){
				int desplazamiento = 0;
				int tamanioBuffer = sizeof(int) + strlen(respuesta->datos) + 1;
				void* buffer = asignarMemoria(tamanioBuffer);
				concatenarString(buffer, &desplazamiento, respuesta->datos);
				enviarMensaje(socketCPU, buffer, tamanioBuffer);
				}else{
					enviarMensaje(socketCPU, FIN_ARCHIVO, sizeof(char));
				}
				freeRespuestaObtencion(respuesta);
				break;
			}

			case LIBERAR_MEMORIA: {
				log_debug(logger, "Liberando memoria");
				break;
			}

			default:
				log_error(logger, "Header desconocido");
				break;
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

void inicializar(char* modo){
	if(strcmp(modo, "SEG_PURA") == 0)
		return inicializarSegPura();

	if(strcmp(modo, "SEG_PAG") == 0)
		return inicializarSegPura(); //CAMBIAAAR

	if(strcmp(modo, "INV") == 0)
		return inicializarSegPura(); //CAMBIAAAR
}

void init(){
	t_config* configuracion = config_create(ARCHIVO_CONFIGURACION);

	tamanioMemoria = config_get_int_value(configuracion, "TAMANIO");
	storage = asignarMemoria(tamanioMemoria);
	char* punteroAModo = config_get_string_value(configuracion, "MODO");
	modo = asignarMemoria(strlen(punteroAModo) + 1);
	memcpy(modo, punteroAModo, strlen(punteroAModo) + 1);

	inicializar(modo);

	offset = 0;
	logger = crearLogger(ARCHIVO_LOG, "FM9");

	colaOperaciones = queue_create();
	inicializarMutex(&mutexOperaciones);
	inicializarSem(&semOperaciones, 0);
	inicializarSem(&semProductores, 0);

	config_destroy(configuracion);
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
