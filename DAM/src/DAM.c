#include "DAM.h"

void inicializarDAM(){
	inicializarMutex(&mutexColaOperaciones);
	inicializarSem(&semHayEnColaOperaciones, 0);
	inicializarSem(&semProductores, 0);
	colaOperaciones = queue_create();
	configuracion = config_create(ARCHIVO_CONFIGURACION);
	transferSize = config_get_int_value(configuracion, "TRANSFER_SIZE");
	logger = crearLogger(ARCHIVO_LOG, "DAM");
}

void entenderMensaje(int emisor, char header){
	int idDTB = deserializarInt(emisor);
	char* path = deserializarString(emisor);
	int cantidadDeLineas;
	switch(header){
		case CARGAR_ESCRIPTORIO_EN_MEMORIA: {
			int esDummy = deserializarInt(emisor); //Si es = 0, es DUMMY
			char header;
			if(esDummy == 0)
				header = CARGAR_ESCRIPTORIO_EN_MEMORIA;
			else
				header = GUARDAR_DATOS;

			log_info(logger, "Ehhh, voy a buscar %s para %d", path, idDTB);
			int validarArchivo = validarArchivoMDJ(path);
			if(validarArchivo != 0){
				printf("Entro al error de validar archivo, valor: %d\n", validarArchivo);
				enviarError(idDTB, path, validarArchivo);
			}else{
				printf("No tiene error, valor: %d\n", validarArchivo);
				char* datos = obtenerDatosDeMDJ(path);
				int estadoDeCarga = enviarDatosAFM9(idDTB, path, datos, header);
				free(datos);
				if(estadoDeCarga != 0){
					enviarError(idDTB, path, estadoDeCarga);
				}else{
					notificarASafaExito(CARGADO_CON_EXITO_EN_MEMORIA,idDTB, path);
				}
			}
			break;
		}

		case GUARDAR_ESCRIPTORIO: {
			log_debug(logger, "Guardando escriptorio");
			pedirDatosAFM9(idDTB, path);
			cantidadDeLineas = deserializarInt(socketFM9);
			char* datos = recibirFlushFM9(cantidadDeLineas);
			int guardarDatos = guardarDatosEnMDJ(datos, path);
			if(guardarDatos != 0){
				enviarError(idDTB, path, guardarDatos);
			}else{
				notificarASafaExito(GUARDADO_CON_EXITO_EN_MDJ,idDTB, path);
			}
			free(datos);
			break;
		}

		case CREAR_ARCHIVO:{
			log_debug(logger, "creando archivo");
			cantidadDeLineas = deserializarInt(emisor);
			int crearArchivo = crearArchivoEnMDJ(socketMDJ, path, cantidadDeLineas);
			if(crearArchivo != 0){
				enviarError(idDTB, path, crearArchivo);
			}else{
				notificarASafaExito(CREADO_CON_EXITO_EN_MDJ,idDTB, path);
			}
			break;
		}

		case BORRAR_ARCHIVO: {
			log_debug(logger, "borrar archivo");
			int borrarArchivo = borrarArchivoEnMDJ(path);
			if(borrarArchivo != 0){
				enviarError(idDTB, path, borrarArchivo);
			}else{
				notificarASafaExito(BORRADO_CON_EXITO_EN_MDJ,idDTB, path);
			}
			break;
		}

		default: {
			log_error(logger, "Header desconocido");
			break;
		}
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

	direccionServidor direccionDAM = levantarDeConfiguracion(NULL, "PUERTO", configuracion);
	int servidorDAM = crearServidor(direccionDAM.puerto, INADDR_ANY);

	//crear servidores para ser cliente de ellos
	direccionServidor direccionSAFA = levantarDeConfiguracion("IP_SAFA", "PUERTO_SAFA", configuracion);
	socketSAFA = conectarConServidor(direccionSAFA.puerto, inet_addr(direccionSAFA.ip));
	printf("Soy SAFA y mi numero de socket es %d\n", socketSAFA);

	direccionServidor direccionMDJ = levantarDeConfiguracion("IP_MDJ", "PUERTO_MDJ", configuracion);
	socketMDJ = conectarConServidor(direccionMDJ.puerto, inet_addr(direccionMDJ.ip));
	printf("Soy MDJ y mi numero de socket es %d\n", socketMDJ);

	direccionServidor direccionFM9 = levantarDeConfiguracion("IP_FM9", "PUERTO_FM9", configuracion);
	socketFM9 = conectarConServidor(direccionFM9.puerto, inet_addr(direccionFM9.ip));
	printf("Soy FM9 y mi numero de socket es %d\n", socketFM9);

	config_destroy(configuracion);

	//mandar handshakes a los servidores
	handshake(socketSAFA, DAM);
	handshake(socketMDJ, DAM);
	handshake(socketFM9, DAM);

	crearSelect(servidorDAM);

	while(1);
	return 0;
}
