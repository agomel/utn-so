#include "FM9.h"
#include "segmentacionPura.h"
#include "segmentacionPag.h"
#include "paginasInvertidas.h"

respuestaDeObtencionDeMemoria* obtenerDatosDeMemoria(int idDTB, char* nombreArchivo){
	if(strcmp(modo, "SEG_PURA") == 0)
		return obtenerDatosSegPura(idDTB, nombreArchivo);

	if(strcmp(modo, "SEG_PAG") == 0)
		return obtenerDatosSegPag(nombreArchivo);

	if(strcmp(modo, "INV") == 0)
		return obtenerDatosInvertida(nombreArchivo);
}

int cargarDatosEnMemoria(int idDTB, char* datos, char* nombreArchivo){
	if(strcmp(modo, "SEG_PURA") == 0)
		return guardarDatosSegPura(idDTB, datos, nombreArchivo);

	if(strcmp(modo, "SEG_PAG") == 0)
		return guardarDatosSegPag(idDTB, datos, nombreArchivo);

	if(strcmp(modo, "INV") == 0)
		return guardarDatosInvertida(idDTB, datos, nombreArchivo);
}

int cargarNuevoDTB(int idDTB, char* datos, char* nombreArchivo){
	if(strcmp(modo, "SEG_PURA") == 0)
		return nuevoProcesoSegPura(idDTB, datos, nombreArchivo);

	if(strcmp(modo, "SEG_PAG") == 0)
		return nuevoProcesoSegPura(idDTB, datos, nombreArchivo); //CAMBIAAAAAAAR

	if(strcmp(modo, "INV") == 0)
		return nuevoProcesoSegPura(idDTB, datos, nombreArchivo); //CAMBIAAAAAAAR
}

respuestaDeObtencionDeMemoria* obtenerLinea(int idDTB, char* nombreArchivo, int numeroLinea){
	if(strcmp(modo, "SEG_PURA") == 0)
		return obtenerLineaSegPura(idDTB, nombreArchivo, numeroLinea);

	if(strcmp(modo, "SEG_PAG") == 0)
		return obtenerLineaSegPag(nombreArchivo, numeroLinea);

	if(strcmp(modo, "INV") == 0)
		return obtenerLineaSegPura(idDTB, nombreArchivo, numeroLinea); //CAMBIAAAAAAAAR
}

void liberarMemoria(int idDTB, char* nombreArchivo){
	if(strcmp(modo, "SEG_PURA") == 0)
		liberarMemoriaSegPura(idDTB, nombreArchivo);

	if(strcmp(modo, "SEG_PAG") == 0)
		liberarMemoriaSegPura(idDTB, nombreArchivo); //CAMBIAAAAAAAAR

	if(strcmp(modo, "INV") == 0)
		liberarMemoriaSegPura(idDTB, nombreArchivo); //CAMBIAAAAAAAAR
}

int asignarDatos(int idDTB, char* nombreArchivo, int numeroLinea, char* datos){
	if(strcmp(modo, "SEG_PURA") == 0)
		return asignarDatosSegPura(idDTB, nombreArchivo, numeroLinea, datos);

	if(strcmp(modo, "SEG_PAG") == 0)
		return asignarDatosSegPura(idDTB, nombreArchivo, numeroLinea, datos); //CAMBIAAAAAAAAAR

	if(strcmp(modo, "INV") == 0)
		return asignarDatosSegPura(idDTB, nombreArchivo, numeroLinea, datos); //CAMBIAAAAAAAAAAAR
}

void freeRespuestaObtencion(respuestaDeObtencionDeMemoria* respuesta){
	free(respuesta->datos);
	free(respuesta);
}

void entenderMensaje(int emisor, char header){
	switch(header){
			case GUARDAR_DATOS: {
				log_debug(logger, "Guardando datos en memoria");
				int idDTB = deserializarInt(emisor);
				char* nombreArchivo = deserializarString(emisor);
				char* datos = deserializarString(emisor);
				int respuestaDeCarga = cargarDatosEnMemoria(idDTB, datos, nombreArchivo);
				free(datos);
				free(nombreArchivo);

				log_debug(logger, "Enviando %d al guardar los datos", respuestaDeCarga);
				enviarYSerializarIntSinHeader(socketDAM, respuestaDeCarga);
				break;
			}

			case CARGAR_ESCRIPTORIO_EN_MEMORIA: {
				int idDTB = deserializarInt(emisor);
				log_debug(logger, "Guardando escriptorio %d en memoria", idDTB);
				char* nombreArchivo = deserializarString(emisor);
				char* datos = deserializarString(emisor);
				int respuestaDeCarga = cargarNuevoDTB(idDTB, datos, nombreArchivo);
				free(datos);
				free(nombreArchivo);

				log_debug(logger, "Enviando %d al cargar nuevo DTB", respuestaDeCarga);
				enviarYSerializarIntSinHeader(socketDAM, respuestaDeCarga);
				break;
			}

			case OBTENER_DATOS: {
				log_debug(logger, "Obteniendo datos de memoria");
				int idDTB = deserializarInt(emisor);
				char* nombreArchivo = deserializarString(emisor);
				respuestaDeObtencionDeMemoria* respuestaDeObtener = obtenerDatosDeMemoria(idDTB, nombreArchivo);

				int desplazamiento = 0;
				int tamanioBuffer = sizeof(int) + sizeof(int) + strlen(respuestaDeObtener->datos) + 1;
				void* buffer = asignarMemoria(tamanioBuffer);
				concatenarInt(buffer, &desplazamiento, respuestaDeObtener->cantidadDeLineas);
				concatenarString(buffer, &desplazamiento, respuestaDeObtener->datos);
				enviarMensaje(socketDAM, buffer, tamanioBuffer);
				free(buffer);
				freeRespuestaObtencion(respuestaDeObtener);
				break;
			}

			case TRAER_LINEA_ESCRIPTORIO: {
				log_debug(logger, "Trayendo linea escriptorio");
				int idDTB = deserializarInt(emisor);
				char* nombreArchivo = deserializarString(emisor);
				int numeroLinea = deserializarInt(emisor);
				respuestaDeObtencionDeMemoria* respuesta = obtenerLinea(idDTB, nombreArchivo, numeroLinea);

				if(respuesta->pudoObtener == 0){
					enviarYSerializarStringSinHeader(emisor, respuesta->datos);
					freeRespuestaObtencion(respuesta);
				}else{
					enviarYSerializarStringSinHeader(emisor, "v/0");
					free(respuesta); //Porque no hay que hacer el free de respuesta->datos
				}

				break;
			}

			case LIBERAR_MEMORIA: {
				int idDTB = deserializarInt(emisor);
				char* nombreArchivo = deserializarString(emisor);
				log_debug(logger, "Liberando memoria para archivo %s", nombreArchivo);
				liberarMemoria(idDTB, nombreArchivo);

				break;
			}

			case ASIGNAR_DATOS: {
				int idDTB = deserializarInt(emisor);
				char* nombreArchivo = deserializarString(emisor);
				int numeroLinea = deserializarInt(emisor);
				char* datos = deserializarString(emisor);
				log_debug(logger, "Escribiendo: %s. En: %s ", datos, nombreArchivo);
				int respuesta = asignarDatos(idDTB, nombreArchivo, numeroLinea, datos);
				enviarYSerializarIntSinHeader(emisor, respuesta);
				free(nombreArchivo);
				free(datos);

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
		return inicializarSegPag();

	if(strcmp(modo, "INV") == 0)
		return inicializarSegPura(); //CAMBIAAAR
}

void init(){
	configuracion = config_create(ARCHIVO_CONFIGURACION);

	tamanioLinea = config_get_int_value(configuracion, "MAX_LINEA");
	tamanioMemoria = config_get_int_value(configuracion, "TAMANIO");
	tamanioPagina = config_get_int_value(configuracion, "TAM_PAGINA");
	storage = asignarMemoria(tamanioMemoria);
	char* punteroAModo = config_get_string_value(configuracion, "MODO");
	modo = asignarMemoria(strlen(punteroAModo) + 1);
	memcpy(modo, punteroAModo, strlen(punteroAModo) + 1);
	cantidadMarcosTotales = tamanioMemoria / tamanioPagina;

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

	configuracion = config_create(ARCHIVO_CONFIGURACION);
	direccionServidor direccionFM9 = levantarDeConfiguracion(NULL, "PUERTO", configuracion);
	int servidor = crearServidor(direccionFM9.puerto, INADDR_ANY);
	crearSelect(servidor);
	config_destroy(configuracion);

	while(1);

	free(storage);
	queue_destroy(colaOperaciones);
	return 0;
}
