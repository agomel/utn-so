#include "MDJ.h"


void entenderMensaje(int emisor, char header){
	char* path;
	int offset;
	int size;
	char* datos;
	char* rutaCompleta;
	int estadoDeOperacion;
	switch(header){
			case VALIDAR_ARCHIVO:
				log_info(logger, "Validar archivo...");
				path = deserializarString(emisor);
				rutaCompleta = concatenar(PUNTO_MONTAJE_ARCHIVOS, path);
				estadoDeOperacion = validarArchivo(rutaCompleta);
				enviarYSerializarIntSinHeader(emisor, estadoDeOperacion);

				log_info(logger, "enviando %d a DAM", estadoDeOperacion);
				free(rutaCompleta);
				free(path);

				break;
			case CREAR_ARCHIVO:
				log_info(logger, "Crear archivo...");
				path = deserializarString(emisor);
				size = deserializarInt(emisor);


				rutaCompleta = concatenar(PUNTO_MONTAJE_ARCHIVOS, path);
				estadoDeOperacion = crearArchivo(rutaCompleta, size);
				enviarYSerializarIntSinHeader(emisor, estadoDeOperacion);

				free(rutaCompleta);
				free(path);
				break;
			case OBTENER_DATOS:
				log_info(logger, "Obtener datos...");
				path = deserializarString(emisor);
				offset = deserializarInt(emisor);
				size = deserializarInt(emisor);

				rutaCompleta = concatenar(PUNTO_MONTAJE_ARCHIVOS, path);
				datos = obtenerDatos(rutaCompleta, offset, size);
				enviarYSerializarStringSinHeader(emisor, datos);

				free(rutaCompleta);
				free(path);
				free(datos);
				break;
			case GUARDAR_DATOS:
				log_info(logger, "Guardar datos...");
				path = deserializarString(emisor);
				offset = deserializarInt(emisor);
				size = deserializarInt(emisor);
				datos = deserializarStringSinElInt(emisor, size);
				size -= 1; //NO quiero que me guarde el \0

				rutaCompleta = concatenar(PUNTO_MONTAJE_ARCHIVOS, path);
				estadoDeOperacion = guardarDatos(rutaCompleta, offset, size, datos);

				enviarYSerializarIntSinHeader(emisor, estadoDeOperacion);

				free(rutaCompleta);
				free(path);
				free(datos);
				break;
			case BORRAR_ARCHIVO:
				log_info(logger, "Borrar archivo...");
				path = deserializarString(emisor);

				rutaCompleta = concatenar(PUNTO_MONTAJE_ARCHIVOS, path);
				estadoDeOperacion = eliminarArchivo(path);

				enviarYSerializarIntSinHeader(emisor, estadoDeOperacion);
				free(rutaCompleta);
				free(path);
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
	char* ubicacionMetadata = concatenar(PUNTO_MONTAJE_METADATA, "Metadata.bin");
	//t_config* configuracion = config_create("mnt/FIFA_FS/Metadata/Metadata.bin");
	t_config* configuracion = config_create(ubicacionMetadata);

	TAMANIO_BLOQUES = config_get_int_value(configuracion, "TAMANIO_BLOQUES");
	CANTIDAD_BLOQUES = config_get_int_value(configuracion, "CANTIDAD_BLOQUES");
	char* magicNumber = config_get_string_value(configuracion, "MAGIC_NUMBER");
	MAGIC_NUMBER = asignarMemoria(strlen(magicNumber) + 1);
	memcpy(MAGIC_NUMBER, magicNumber, strlen(magicNumber)+ 1);
	free(magicNumber);

	//config_destroy(configuracion);

}

void obtenerPuntoMontaje(char* primerMontaje){
	PUNTO_MONTAJE = asignarMemoria(250);
	char* path = malloc(250);
	getcwd(path, 250);
	PUNTO_MONTAJE = concatenar(path, primerMontaje);
	free(path);
	log_info(logger, "el montaje es %s", PUNTO_MONTAJE);
}

void crearPuntosDeMontaje(){
	PUNTO_MONTAJE_ARCHIVOS = concatenar(PUNTO_MONTAJE, "Archivos/");
	PUNTO_MONTAJE_METADATA = concatenar(PUNTO_MONTAJE, "Metadata/");
	PUNTO_MONTAJE_BLOQUES = concatenar(PUNTO_MONTAJE, "Bloques/");
}
void init(){
	t_config* configuracion = config_create(ARCHIVO_CONFIGURACION);
	RETARDO = config_get_int_value(configuracion, "RETARDO");
	char* punteroPuntoMontaje = config_get_string_value(configuracion, "PUNTO_MONTAJE");
	MONTAJE_ACTUAL = asignarMemoria(250);
	memcpy(MONTAJE_ACTUAL, punteroPuntoMontaje, strlen(punteroPuntoMontaje)+ 1);
	free(punteroPuntoMontaje);
	//config_destroy(configuracion);

	logger = crearLogger(ARCHIVO_LOG, "MDJ");

	obtenerPuntoMontaje(MONTAJE_ACTUAL);

	crearPuntosDeMontaje();

	levantarMetadata();

	verificarExistenciaDeBitmap();


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
