#include "DAM.h"

void inicializarDAM(){
	inicializarMutex(&mutexColaOperaciones);
	inicializarSem(&semHayEnColaOperaciones, 0);
	inicializarSem(&semProductores, 0);
	colaOperaciones = queue_create();
	configuraciones = config_create(ARCHIVO_CONFIGURACION);
	transferSize = config_get_int_value(configuraciones, "TRANSFER_SIZE");
	logger = crearLogger(ARCHIVO_LOG, "DAM");
}

void entenderMensaje(int emisor, char header){
	int idDTB = deserializarInt(emisor);
	char* path = deserializarString(emisor);

	switch(header){
		case CARGAR_ESCRIPTORIO_EN_MEMORIA:
			log_info(logger, "Ehhh, voy a buscar %s para %d", path, idDTB);
			int validarArchivo = validarArchivoMDJ(path);
			if(validarArchivo != 0){
				enviarError(idDTB, path, validarArchivo);
			}else {
				char* datos = obtenerDatosDeMDJ(path);
				int estadoDeCarga = enviarDatosAFM9(datos);
				if(estadoDeCarga != 0){
					enviarError(idDTB, path, estadoDeCarga);
				}else{
					t_list* direcciones = recibirListaDeDireccionesDeFM9();
					log_info(logger,"Recibio la lista de direcciones");
					notificarASafaExitoDeCarga(idDTB, path, direcciones);
				}
			}
			break;
		case GUARDAR_ESCRIPTORIO:
			log_debug(logger, "Guardando escriptorio");
			t_list* direcciones = deserializarListaInt(emisor);
			int estadoDeOperacion = pedirDatosAFM9(direcciones);
			if(estadoDeOperacion != 0){
				enviarError(idDTB, path, estadoDeOperacion);
			}else{
				int cantidadDeLineas = deserializarInt(socketFM9);
				char* datos = recibirFlushFM9(transferSize);
				int crearArchivo = crearArchivoEnMDJ(socketMDJ, path, cantidadDeLineas);

				if(crearArchivo != 0){
					enviarError(idDTB, path, crearArchivo);
				}else{
					int guardarDatos = guardarDatosEnMDJ(datos);
					if(guardarDatos != 0){
						enviarError(idDTB, path, guardarDatos);
					}else{
						notificarASafaExitoDeGuardado(idDTB, path);
					}
				}
				free(datos);
			}
			break;
		case CREAR_ARCHIVO:
			log_debug(logger, "creando archivo");
			int cantidadDeLineas = deserializarInt(emisor);
			int crearArchivo = crearArchivoEnMDJ(socketMDJ, path, cantidadDeLineas);
			if(crearArchivo != 0){
				enviarError(idDTB, path, crearArchivo);
			}
			break;
		case BORRAR_ARCHIVO:
			log_debug(logger, "borrar archivo");
			int borrarArchivo = borrarArchivoEnMDJ(path);
			if(borrarArchivo != 0){
				enviarError(idDTB, path, borrarArchivo);
			}
			break;
		default:
			log_error(logger, "Header desconocido");
	}
}

int identificarse(int emisor, char header){
	if(header == IDENTIFICARSE){
		char identificado = deserializarChar(emisor);
		log_debug(logger, "Handshake de: %s", traducirModulo(identificado));
		if(identificado == CPU)
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
	select->mutexOperaciones = &mutexColaOperaciones;
	select->semOperaciones = &semHayEnColaOperaciones;
	select->socket = servidor;
	select->identificarse = &identificarse;
	select->semProductores = &semProductores;
	realizarNuestroSelect(select);
}

int main(void) {
	inicializarDAM();
	t_config* configuracion = config_create(ARCHIVO_CONFIGURACION);

	direccionServidor direccionDAM = levantarDeConfiguracion(NULL, "PUERTO", configuracion);
	int servidorDAM = crearServidor(direccionDAM.puerto, INADDR_ANY);

	//crear servidores para ser cliente de ellos
	direccionServidor direccionSAFA = levantarDeConfiguracion("IP_SAFA", "PUERTO_SAFA", configuracion);
	socketSAFA = conectarConServidor(direccionSAFA.puerto, inet_addr(direccionSAFA.ip));

	direccionServidor direccionMDJ = levantarDeConfiguracion("IP_MDJ", "PUERTO_MDJ", configuracion);
	socketMDJ = conectarConServidor(direccionMDJ.puerto, inet_addr(direccionMDJ.ip));

	direccionServidor direccionFM9 = levantarDeConfiguracion("IP_FM9", "PUERTO_FM9", configuracion);
	socketFM9 = conectarConServidor(direccionFM9.puerto, inet_addr(direccionFM9.ip));

	config_destroy(configuracion);

	//mandar handshakes a los servidores
	handshake(socketSAFA, DAM);
	handshake(socketMDJ, DAM);
	handshake(socketFM9, DAM);

	crearSelect(servidorDAM);

	while(1);
	return 0;
}
