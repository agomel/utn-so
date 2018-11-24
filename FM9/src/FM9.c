#include "FM9.h"
#include "segmentacionPura.h"
#include "segmentacionPag.h"
#include "paginasInvertidas.h"
#include "consola.h"

respuestaDeObtencionDeMemoria* obtenerDatosDeMemoria(int idDTB, char* nombreArchivo){
	if(strcmp(modo, "SEG_PURA") == 0)
		return obtenerDatosSegPura(idDTB, nombreArchivo);

	if(strcmp(modo, "SEG_PAG") == 0)
		return obtenerDatosSegPag(idDTB, nombreArchivo);

	if(strcmp(modo, "INV") == 0)
		return obtenerDatosInvertida(idDTB, nombreArchivo);
}

RespuestaGuardado* cargarDatosEnMemoria(int idDTB, char* datos, char* nombreArchivo){
	if(strcmp(modo, "SEG_PURA") == 0)
		return guardarDatosSegPura(idDTB, datos, nombreArchivo);

	if(strcmp(modo, "SEG_PAG") == 0)
		return guardarDatosSegPag(idDTB, datos, nombreArchivo);

	if(strcmp(modo, "INV") == 0)
		return guardarDatosInvertida(idDTB, datos, nombreArchivo);
}

RespuestaGuardado* cargarNuevoDTB(int idDTB, char* datos, char* nombreArchivo){
	if(strcmp(modo, "SEG_PURA") == 0)
		return nuevoProcesoSegPura(idDTB, datos, nombreArchivo);

	if(strcmp(modo, "SEG_PAG") == 0)
		return nuevoProcesoSegPag(idDTB, datos, nombreArchivo);

	if(strcmp(modo, "INV") == 0)
		return guardarDatosInvertida(idDTB, datos, nombreArchivo);
}

respuestaDeObtencionDeMemoria* obtenerLinea(int idDTB, char* nombreArchivo, int numeroLinea){
	if(strcmp(modo, "SEG_PURA") == 0)
		return obtenerLineaSegPura(idDTB, nombreArchivo, numeroLinea);

	if(strcmp(modo, "SEG_PAG") == 0)
		return obtenerLineaSegPag(idDTB, nombreArchivo, numeroLinea);

	if(strcmp(modo, "INV") == 0)
		return obtenerLineaInvertida(idDTB, nombreArchivo, numeroLinea);
}

void liberarMemoria(int idDTB, char* nombreArchivo){
	if(strcmp(modo, "SEG_PURA") == 0)
		liberarMemoriaSegPura(idDTB, nombreArchivo);

	if(strcmp(modo, "SEG_PAG") == 0)
		liberarMemoriaSegPag(idDTB, nombreArchivo);

	if(strcmp(modo, "INV") == 0)
		liberarMemoriaInvertida(idDTB, nombreArchivo);
}

void liberarDTBDeMemoria(int idDTB){
	if(strcmp(modo, "SEG_PURA") == 0)
		liberarDTBDeMemoriaSegPura(idDTB);//PROBAR

	if(strcmp(modo, "SEG_PAG") == 0)
		liberarDTBDeMemoriaSegPag(idDTB);//PROBAR

	if(strcmp(modo, "INV") == 0)
		liberarDTBDeMemoriaInvertida(idDTB);
}

int asignarDatos(int idDTB, char* nombreArchivo, int numeroLinea, char* datos){
	if(strcmp(modo, "SEG_PURA") == 0)
		return asignarDatosSegPura(idDTB, nombreArchivo, numeroLinea, datos);

	if(strcmp(modo, "SEG_PAG") == 0)
		return asignarDatosSegPag(idDTB, nombreArchivo, numeroLinea, datos);

	if(strcmp(modo, "INV") == 0)
		return asignarDatosInvertida(idDTB, nombreArchivo, numeroLinea, datos);
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
				RespuestaGuardado* respuestaDeCarga = cargarDatosEnMemoria(idDTB, datos, nombreArchivo);
				free(datos);
				free(nombreArchivo);

				log_debug(logger, "Enviando %d al guardar los datos", respuestaDeCarga->pudoGuardar);
				enviarYSerializarIntSinHeader(socketDAM, respuestaDeCarga->pudoGuardar);
				if(respuestaDeCarga->pudoGuardar == 0)
					enviarYSerializarIntSinHeader(socketDAM, respuestaDeCarga->pesoArchivo);

				free(respuestaDeCarga);

				break;
			}

			case CARGAR_ESCRIPTORIO_EN_MEMORIA: {
				int idDTB = deserializarInt(emisor);
				log_debug(logger, "Guardando escriptorio %d en memoria", idDTB);
				char* nombreArchivo = deserializarString(emisor);
				char* datos = deserializarString(emisor);
				RespuestaGuardado* respuestaDeCarga = cargarNuevoDTB(idDTB, datos, nombreArchivo);
				free(datos);
				free(nombreArchivo);

				log_debug(logger, "Enviando %d al cargar nuevo DTB", respuestaDeCarga->pudoGuardar);
				enviarYSerializarIntSinHeader(socketDAM, respuestaDeCarga->pudoGuardar);
				if(respuestaDeCarga->pudoGuardar == 0)
					enviarYSerializarIntSinHeader(socketDAM, respuestaDeCarga->pesoArchivo);

				free(respuestaDeCarga);
				break;
			}

			case OBTENER_DATOS: {
				log_debug(logger, "Obteniendo datos de memoria");
				int idDTB = deserializarInt(emisor);
				char* nombreArchivo = deserializarString(emisor);
				respuestaDeObtencionDeMemoria* respuestaDeObtener = obtenerDatosDeMemoria(idDTB, nombreArchivo);

				int desplazamiento = 0;
				int tamanioBuffer = sizeof(int) + strlen(respuestaDeObtener->datos) + 1;
				void* buffer = asignarMemoria(tamanioBuffer);
				concatenarString(buffer, &desplazamiento, respuestaDeObtener->datos);
				log_info(logger, "Datos enviados: %s ", respuestaDeObtener->datos);
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
				log_debug(logger, "Se pide linea %d para escriptorio %s", numeroLinea, nombreArchivo);
				respuestaDeObtencionDeMemoria* respuesta = obtenerLinea(idDTB, nombreArchivo, numeroLinea);

				if(respuesta->pudoObtener == 0){
					enviarYSerializarStringSinHeader(emisor, respuesta->datos);
					freeRespuestaObtencion(respuesta);
				}else if(respuesta->pudoObtener == 3){
					//END OF FILE
					log_info(logger, "Es fin de archivo");
					char* rta = string_new();
					string_append(&rta, "v"); //"v" = FIN_ARCHIVO
					enviarYSerializarStringSinHeader(emisor, rta);
					free(rta);
					free(respuesta); //Porque no hay que hacer el free de respuesta->datos

				}else{
					char* rta = string_new();
					string_append(&rta, "u"); //Error o acceso invalido
					enviarYSerializarStringSinHeader(emisor, rta);
					free(rta);
					free(respuesta); //Porque no hay que hacer el free de respuesta->datos
				}

				break;
			}

			case LIBERAR_MEMORIA: {
				int idDTB = deserializarInt(emisor);
				char* nombreArchivo = deserializarString(emisor);
				log_debug(logger, "Liberando memoria para archivo %s", nombreArchivo);
				liberarMemoria(idDTB, nombreArchivo);
				enviarYSerializarCharSinHeader(emisor, 'a');

				break;
			}

			case LIBERAR_DTB_MEMORIA: {
				int idDTB = deserializarInt(emisor);
				log_debug(logger, "Liberando el dtb %d de memoria por pasar a EXIT", idDTB);
				liberarDTBDeMemoria(idDTB);
				enviarYSerializarCharSinHeader(emisor, 'a');

				break;
			}

			case ASIGNAR_DATOS: {
				int idDTB = deserializarInt(emisor);
				char* nombreArchivo = deserializarString(emisor);
				int numeroLinea = deserializarInt(emisor);
				log_debug(logger, "Asignando datos en linea Nro %d", numeroLinea);
				char* datos = deserializarString(emisor);
				log_debug(logger, "Escribiendo: %s. En: %s ", datos, nombreArchivo);
				int respuesta = asignarDatos(idDTB, nombreArchivo, numeroLinea, datos);
				enviarYSerializarIntSinHeader(emisor, respuesta);
				free(nombreArchivo);
				free(datos);

				break;
			}
			case FINALIZARME:
				exit(1);
				break;
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

void inicializar(char* modo, t_config* configuracion){
	if(strcmp(modo, "SEG_PURA") == 0){
		log_info(logger, "Utilizando segmentacion pura");
		return inicializarSegPura();
	}

	if(strcmp(modo, "SEG_PAG") == 0){
		log_info(logger, "Utilizando segmentacion paginada");
		return inicializarSegPag(configuracion);
	}

	if(strcmp(modo, "INV") == 0){
		log_info(logger, "Utilizando paginas invertidas");
		return inicializarInvertida(configuracion);
	}
}

void init(){
	configuracion = config_create(ARCHIVO_CONFIGURACION);

	tamanioLinea = config_get_int_value(configuracion, "MAX_LINEA");
	tamanioMemoria = config_get_int_value(configuracion, "TAMANIO");
	storage = asignarMemoria(tamanioMemoria);
	char* punteroAModo = config_get_string_value(configuracion, "MODO");
	modo = asignarMemoria(strlen(punteroAModo) + 1);
	memcpy(modo, punteroAModo, strlen(punteroAModo) + 1);
	logger = crearLogger(ARCHIVO_LOG, "FM9");

	inicializar(modo, configuracion);

	offset = 0;


	colaOperaciones = queue_create();
	inicializarMutex(&mutexOperaciones);
	inicializarSem(&semOperaciones, 0);
	inicializarSem(&semProductores, 0);
}

void despedida(){
	log_info(logger, "chauuuuu :)");
	enviarYSerializarCharSinHeader(socketDAM, FINALIZARME);
	raise(SIGTERM);
}

int main(void) {
	signal(SIGINT, despedida);
	init();
	direccionServidor direccionFM9 = levantarDeConfiguracion(NULL, "PUERTO", configuracion);
	int servidor = crearServidor(direccionFM9.puerto, INADDR_ANY);
	crearSelect(servidor);
	config_destroy(configuracion);

	pthread_t hiloConsola = crearHilo(&consola, NULL);
	esperarHilo(hiloConsola);


	return 0;
}
