#include "MDJ.h"


void entenderMensaje(int emisor, char header){
	char* datos;
	usleep(RETARDO*1000);//tiempo en milisegundos

	switch(header){
			case VALIDAR_ARCHIVO:
				log_info(logger, "validando archivo de emisor %d", emisor);
				int archivoValido = validarArchivo(emisor);
				enviarYSerializarIntSinHeader(emisor, archivoValido);
				break;
			case CREAR_ARCHIVO:
				crearArchivo(emisor);
				break;
			case OBTENER_DATOS:
				datos = obtenerDatos(emisor);
				enviarYSerializarStringSinHeader(emisor, datos);
				break;
			case GUARDAR_DATOS:
				guardarDatos(emisor);
				break;
		default:
			log_error(logger, "Header desconocido");
	}
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
	PUNTO_MONTAJE = asignarMemoria(strlen(punteroPuntoMontaje) + 1);
	memcpy(PUNTO_MONTAJE, punteroPuntoMontaje, strlen(punteroPuntoMontaje)+ 1);
	free(punteroPuntoMontaje);
	config_destroy(configuracion);

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
