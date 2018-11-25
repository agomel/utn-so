#include "MDJ.h"


void entenderMensaje(int emisor, char header){
	char* path;
	int offset;
	int size;
	char* datos;
	int estadoDeOperacion;
	switch(header){
			case VALIDAR_ARCHIVO: {
				log_info(logger, "Validar archivo...");
				path = deserializarString(emisor);

				estadoDeOperacion = validarArchivoFIFA(path);

				enviarYSerializarIntSinHeader(emisor, estadoDeOperacion);

				free(path);
				break;
			}
			case CREAR_ARCHIVO: {
				log_info(logger, "Crear archivo...");
				path = deserializarString(emisor);
				size = deserializarInt(emisor);

				estadoDeOperacion = crearArchivoFIFA(path, size);

				enviarYSerializarIntSinHeader(emisor, estadoDeOperacion);

				free(path);
				break;
			}
			case OBTENER_DATOS: {
				log_info(logger, "Obtener datos...");
				path = deserializarString(emisor);
				offset = deserializarInt(emisor);
				size = deserializarInt(emisor);

				datos = obtenerDatosFIFA(path, offset, size);

				log_info(logger, "Datos a enviar: %s", datos);
				enviarYSerializarStringSinHeader(emisor, datos);

				free(path);
				free(datos);
				break;
			}
			case GUARDAR_DATOS: {
				log_info(logger, "Guardar datos...");
				path = deserializarString(emisor);
				offset = deserializarInt(emisor);
				datos = deserializarString(emisor);
				size = strlen(datos); //NO quiero que me guarde el \0

				estadoDeOperacion = guardarDatosFIFA(path, offset, size, datos);

				log_debug(logger, "Enviando %d al guardar datos", estadoDeOperacion);

				enviarYSerializarIntSinHeader(emisor, estadoDeOperacion);

				free(path);
				free(datos);
				break;
			}
			case BORRAR_ARCHIVO: {
				log_info(logger, "Borrar archivo...");
				path = deserializarString(emisor);

				estadoDeOperacion = borrarArchivoFIFA(path);

				enviarYSerializarIntSinHeader(emisor, estadoDeOperacion);
				free(path);
				break;
			}
			case FINALIZARME:
				persistirBitMap();
				exit(1);
				break;
		default:
			log_error(logger, "Header desconocido");
	}
	usleep(RETARDO*1000);//tiempo en milisegundos
}

int identificarse(int emisor, char header){
	if(header == IDENTIFICARSE){
		char identificado = deserializarChar(emisor);
		log_debug(logger, "Handshake de: %s", traducirModulo(identificado));
		if(identificado == DAM){
			return 1;
			socketDAM = emisor;
		}
	}
	log_error(logger, "Conexion rechazada");
	return 0;
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
void levantarMetadata(){
	char* ubicacionMetadata = concatenar(PUNTO_MONTAJE_METADATA, "Metadata.bin");

	t_config* metadataConfig = config_create(ubicacionMetadata);

	TAMANIO_BLOQUES = config_get_int_value(metadataConfig, "TAMANIO_BLOQUES");
	CANTIDAD_BLOQUES = config_get_int_value(metadataConfig, "CANTIDAD_BLOQUES");
	char* magicNumber = config_get_string_value(metadataConfig, "MAGIC_NUMBER");
	MAGIC_NUMBER = asignarMemoria(strlen(magicNumber) +1);
	memcpy(MAGIC_NUMBER, magicNumber, strlen(magicNumber) +1);
	free(magicNumber);

	//config_destroy(metadataConfig);

}

void obtenerPuntoMontaje(char* primerMontaje){
	PUNTO_MONTAJE = asignarMemoria(250);
	char* path = asignarMemoria(250);
	getcwd(path, 250);
	memcpy(PUNTO_MONTAJE, path, strlen(path) +1);
	string_append_with_format(&PUNTO_MONTAJE, "%s\0", primerMontaje);
	free(path);
}

void crearPuntosDeMontaje(){
	PUNTO_MONTAJE_ARCHIVOS = concatenar(PUNTO_MONTAJE, "Archivos/");
	PUNTO_MONTAJE_METADATA = concatenar(PUNTO_MONTAJE, "Metadata/");
	PUNTO_MONTAJE_BLOQUES = concatenar(PUNTO_MONTAJE, "Bloques/");

}
void despedida(){
	log_info(logger, "chauuuuu :)");
	enviarYSerializarCharSinHeader(socketDAM, FINALIZARME);
	raise(SIGTERM);
}
void init(){
	signal(SIGINT, despedida);
	configuracion = config_create(ARCHIVO_CONFIGURACION);
	RETARDO = config_get_int_value(configuracion, "RETARDO");
	char* punteroPuntoMontaje = config_get_string_value(configuracion, "PUNTO_MONTAJE");
	MONTAJE_ACTUAL = asignarMemoria(250);
	memcpy(MONTAJE_ACTUAL, punteroPuntoMontaje, strlen(punteroPuntoMontaje) + 1);
	free(punteroPuntoMontaje);
	//config_destroy(configuracion);

	logger = crearLogger(ARCHIVO_LOG, "MDJ");

	obtenerPuntoMontaje(MONTAJE_ACTUAL);

	crearPuntosDeMontaje();

	levantarMetadata();

	initBitmap();

	crearArchivoDePruebas();

	inicializarMutex(&mutexOperaciones);
	colaOperaciones = queue_create();
	inicializarSem(&semOperaciones, 0);
	inicializarSem(&semProductores, 0);

	crearHiloQueMuereSolo(consolita, NULL);
}

int main(void) {
	init();

	direccionServidor direccionMDJ = levantarDeConfiguracion(NULL, "PUERTO", configuracion);
	int servidor = crearServidor(direccionMDJ.puerto, INADDR_ANY);
	//config_destroy(configuracion);

	crearSelect(servidor);


	while(1);

	return 0;
}
