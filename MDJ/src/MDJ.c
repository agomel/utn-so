#include "MDJ.h"


void entenderMensaje(int emisor, char header){
	char* path;
	int offset;
	int size;
	char* datos;

	int estadoDeOperacion;

	switch(header){
			case VALIDAR_ARCHIVO:
				log_info(logger, "Validar archivo...");
				path = deserializarString(emisor);

				estadoDeOperacion = validarArchivo(path);

				enviarYSerializarIntSinHeader(emisor, estadoDeOperacion);

				free(path);
				break;
			case CREAR_ARCHIVO:
				log_info(logger, "Crear archivo...");
				path = deserializarString(emisor);
				size = deserializarInt(emisor);

				estadoDeOperacion = crearArchivo(path, size);

				enviarYSerializarIntSinHeader(emisor, estadoDeOperacion);

				free(path);
				break;
			case OBTENER_DATOS:
				log_info(logger, "Obtener datos...");
				path = deserializarString(emisor);
				offset = deserializarInt(emisor);
				size = deserializarInt(emisor);

				datos = obtenerDatos(path, offset, size);

				enviarYSerializarStringSinHeader(emisor, datos);

				free(path);
				free(datos);
				break;
			case GUARDAR_DATOS:
				log_info(logger, "Guardar datos...");
				path = deserializarString(emisor);
				offset = deserializarInt(emisor);
				size = deserializarInt(emisor);
				datos = deserializarStringSinElInt(emisor, size);

				estadoDeOperacion = guardarDatos(path, offset, size, datos);

				enviarYSerializarIntSinHeader(emisor, estadoDeOperacion);

				free(path);
				free(datos);
				break;
			case BORRAR_ARCHIVO:
				log_info(logger, "Borrar archivo...");
				path = deserializarString(emisor);

				estadoDeOperacion = eliminarArchivo(path);

				enviarYSerializarIntSinHeader(emisor, estadoDeOperacion);

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

void init(){
	t_config* configuracion = config_create(ARCHIVO_CONFIGURACION);
	RETARDO = config_get_int_value(configuracion, "RETARDO");
	char* punteroPuntoMontaje = config_get_string_value(configuracion, "PUNTO_MONTAJE");
	PUNTO_MONTAJE = malloc(250);//asignarMemoria(strlen(punteroPuntoMontaje) + 1);
	memcpy(PUNTO_MONTAJE, punteroPuntoMontaje, strlen(punteroPuntoMontaje)+ 1);
	free(punteroPuntoMontaje);
	//config_destroy(configuracion);

	logger = crearLogger(ARCHIVO_LOG, "MDJ");

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
