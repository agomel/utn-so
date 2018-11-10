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
				char* rutaCompleta = asignarMemoria(strlen(PUNTO_MONTAJE) + 1);
				memcpy(rutaCompleta, PUNTO_MONTAJE, strlen(PUNTO_MONTAJE) + 1);
				string_append(&rutaCompleta, path);
				estadoDeOperacion = validarArchivo(rutaCompleta);
				enviarYSerializarIntSinHeader(emisor, estadoDeOperacion);

				log_info(logger, "enviando %d a DAM", estadoDeOperacion);
				free(rutaCompleta);
				free(path);

				break;
			}
			case CREAR_ARCHIVO: {
				log_info(logger, "Crear archivo...");
				path = deserializarString(emisor);
				size = deserializarInt(emisor);

				char* rutaCompleta = asignarMemoria(strlen(PUNTO_MONTAJE) + 1);
				memcpy(rutaCompleta, PUNTO_MONTAJE, strlen(PUNTO_MONTAJE) + 1);
				string_append(&rutaCompleta, path);
				estadoDeOperacion = crearArchivo(rutaCompleta, size);
				enviarYSerializarIntSinHeader(emisor, estadoDeOperacion);

				free(rutaCompleta);
				free(path);
				break;
			}
			case OBTENER_DATOS: {
				log_info(logger, "Obtener datos...");
				path = deserializarString(emisor);
				offset = deserializarInt(emisor);
				size = deserializarInt(emisor);

				char* rutaCompleta = asignarMemoria(strlen(PUNTO_MONTAJE) + 1);
				memcpy(rutaCompleta, PUNTO_MONTAJE, strlen(PUNTO_MONTAJE) + 1);
				string_append(&rutaCompleta, path);
				datos = obtenerDatos(rutaCompleta, offset, size);
				enviarYSerializarStringSinHeader(emisor, datos);

				free(rutaCompleta);
				free(path);
				free(datos);
				break;
			}
			case GUARDAR_DATOS: {
				log_info(logger, "Guardar datos...");
				path = deserializarString(emisor);
				offset = deserializarInt(emisor);
				size = deserializarInt(emisor);
				datos = deserializarStringSinElInt(emisor, size);
				size -= 1; //NO quiero que me guarde el \0

				char* rutaCompleta = asignarMemoria(strlen(PUNTO_MONTAJE) + 1);
				memcpy(rutaCompleta, PUNTO_MONTAJE, strlen(PUNTO_MONTAJE) + 1);
				string_append(&rutaCompleta, path);
				estadoDeOperacion = guardarDatos(rutaCompleta, offset, size, datos);

				enviarYSerializarIntSinHeader(emisor, estadoDeOperacion);

				free(rutaCompleta);
				free(path);
				free(datos);
				break;
			}
			case BORRAR_ARCHIVO: {
				log_info(logger, "Borrar archivo...");
				path = deserializarString(emisor);

				char* rutaCompleta = asignarMemoria(strlen(PUNTO_MONTAJE) + 1);
				memcpy(rutaCompleta, PUNTO_MONTAJE, strlen(PUNTO_MONTAJE) + 1);
				string_append(&rutaCompleta, path);
				estadoDeOperacion = eliminarArchivo(path);

				enviarYSerializarIntSinHeader(emisor, estadoDeOperacion);
				free(rutaCompleta);
				free(path);
				break;
			}
		default:
			log_error(logger, "Header desconocido");
	}
	usleep(RETARDO*1000);//tiempo en milisegundos
}

int identificarse(int emisor, char header){
	if(header == IDENTIFICARSE){
		char identificado = deserializarChar(emisor);
		log_debug(logger, "Handshake de: %s", traducirModulo(identificado));
		if(identificado == DAM)
			return 1;
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
	t_config* configuracion = config_create("mnt/FIFA_FS/Metadata/Metadata.bin");

	TAMANIO_BLOQUES = config_get_int_value(configuracion, "TAMANIO_BLOQUES");
	CANTIDAD_BLOQUES = config_get_int_value(configuracion, "CANTIDAD_BLOQUES");
	char* magicNumber = config_get_string_value(configuracion, "MAGIC_NUMBER");
	MAGIC_NUMBER = asignarMemoria(strlen(magicNumber) +1);
	memcpy(MAGIC_NUMBER, magicNumber, strlen(magicNumber) +1);
	free(magicNumber);

	//config_destroy(configuracion);

}

void obtenerPuntoMontaje(char* primerMontaje){
	PUNTO_MONTAJE = asignarMemoria(250);
	char* path = asignarMemoria(250);
	getcwd(path, 250);
	memcpy(PUNTO_MONTAJE, path, strlen(path) +1);
	string_append_with_format(&PUNTO_MONTAJE, "%s\0", primerMontaje);
	free(path);
	log_info(logger, "el montaje es %s", PUNTO_MONTAJE);
}

void init(){
	t_config* configuracion = config_create(ARCHIVO_CONFIGURACION);
	RETARDO = config_get_int_value(configuracion, "RETARDO");
	char* punteroPuntoMontaje = config_get_string_value(configuracion, "PUNTO_MONTAJE");
	MONTAJE_ACTUAL = asignarMemoria(250);
	memcpy(MONTAJE_ACTUAL, punteroPuntoMontaje, strlen(punteroPuntoMontaje) + 1);
	free(punteroPuntoMontaje);
	//config_destroy(configuracion);


	levantarMetadata();

	logger = crearLogger(ARCHIVO_LOG, "MDJ");


	obtenerPuntoMontaje(MONTAJE_ACTUAL);

	inicializarMutex(&mutexOperaciones);
	colaOperaciones = queue_create();
	inicializarSem(&semOperaciones, 0);
	inicializarSem(&semProductores, 0);

	crearHiloQueMuereSolo(consolita, NULL);
}
int main(void) {
	init();

	t_config* configuracion = config_create(ARCHIVO_CONFIGURACION);

	direccionServidor direccionMDJ = levantarDeConfiguracion(NULL, "PUERTO", configuracion);
	int servidor = crearServidor(direccionMDJ.puerto, INADDR_ANY);
	config_destroy(configuracion);

	crearSelect(servidor);

	while(1);

	return 0;
}
