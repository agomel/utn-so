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
int validarArchivoMDJ(char* path){
	int tamanioMensaje = sizeof(char) + sizeof(int) + strlen(path)+1;
	void* buffer = asignarMemoria(tamanioMensaje);
	int desplazamiento = 0;
	concatenarChar(buffer, &desplazamiento, VALIDAR_ARCHIVO);
	concatenarString(buffer, &desplazamiento, path);
	log_info(logger, "enviando a emisor %d char: %c y escriptorio %s", socketMDJ, VALIDAR_ARCHIVO, path);
	enviarMensaje(socketMDJ, buffer, tamanioMensaje);
	free(buffer);
	int respuesta =  deserializarInt(socketMDJ);
	return respuesta;

}
char* obtenerDatosDeMDJ(){
	int offset = 0;
	/*char* datosTotales = asignarMemoria(0);

	int cantidadDeDatos = transferSize;
	while(cantidadDeDatos == transferSize){
		void* buffer = asignarMemoria(sizeof(char) + sizeof(int) + (strlen(operacion->path)+1)
				+ sizeof(int) + sizeof(int));
		int desplazamiento = 0;

		concatenarChar(buffer, &desplazamiento, OBTENER_DATOS);
		concatenarString(buffer, &desplazamiento, operacion->path);
		concatenarInt(buffer, &desplazamiento, offset);
		concatenarInt(buffer, &desplazamiento, transferSize);

		enviarMensaje(socketMDJ, buffer, desplazamiento);
		free(buffer);

		cantidadDeDatos = deserializarInt(socketMDJ);
		char* datosParciales = asignarMemoria(cantidadDeDatos);

		datosTotales = realloc(datosTotales, sizeof(datosTotales) + cantidadDeDatos);
		recibirMensaje(socketMDJ, datosParciales, cantidadDeDatos);

		offset = offset + cantidadDeDatos;
		//TODO ver porque rompe esto
	}*/
	//ESTO es jarcodeado
	char* datosTotales = "estos son los datos";
	return datosTotales;
}

int crearArchivoEnMDJ(char* path, int cantidadDeLineas){
	void* buffer = asignarMemoria(sizeof(char) + sizeof(int) + (strlen(path)+1) + sizeof(int));
	int desplazamiento = 0;
	concatenarChar(buffer, &desplazamiento, CREAR_ARCHIVO);
	concatenarString(buffer, &desplazamiento, path);
	concatenarInt(buffer, &desplazamiento, cantidadDeLineas);
	enviarMensaje(socketMDJ, buffer, desplazamiento);
	free(buffer);
	return deserializarInt(socketMDJ);
}
int guardarDatosEnMDJ(void* datosTotales){
	int cantidadTotal = sizeof(datosTotales);
	enviarYSerializarInt(socketMDJ, cantidadTotal, GUARDAR_DATOS);
	int desplazamiento = 0;
	while(cantidadTotal > 0){
		int cantidadAEnviar;
		if(cantidadTotal > transferSize){
			cantidadAEnviar = transferSize;
		}else{
			cantidadAEnviar = cantidadTotal;
		}
		enviarMensaje(socketMDJ, datosTotales + desplazamiento, cantidadAEnviar);
		desplazamiento = desplazamiento + cantidadAEnviar;
		cantidadTotal = cantidadTotal - cantidadAEnviar;
	}
	return deserializarInt(socketMDJ);
}
void notificarASafaExitoDeCarga(int idDTB, char* path, t_list* direcciones){
	int tamanio = sizeof(char) + sizeof(int)*2 + strlen(path)+1 + sizeof(int) + direcciones->elements_count*sizeof(int);
	void* buffer = asignarMemoria(tamanio);
	int desplazamiento = 0;
	concatenarChar(buffer, &desplazamiento, CARGADO_CON_EXITO_EN_MEMORIA);
	concatenarInt(buffer, &desplazamiento, idDTB);
	concatenarString(buffer, &desplazamiento, path);
	concatenarListaInt(buffer, &desplazamiento, direcciones);
	enviarMensaje(socketSAFA, buffer, tamanio);
	free(buffer);
}
void notificarASafaExitoDeGuardado(int idDTB, char* path){
	void* buffer = asignarMemoria(sizeof(char) + sizeof(int) + strlen(path)+1);
	int desplazamiento = 0;
	concatenarChar(buffer, &desplazamiento, GUARDADO_CON_EXITO_EN_MDJ);
	concatenarInt(buffer, &desplazamiento, idDTB);
	concatenarString(buffer, &desplazamiento, path);
	enviarMensaje(socketSAFA, buffer, desplazamiento);
	free(buffer);
}
void enviarError(int idDTB, char* path, int error){
	void* buffer = asignarMemoria(sizeof(char) + sizeof(int) + strlen(path)+1);
	int desplazamiento = 0;
	concatenarChar(buffer, &desplazamiento, ERROR);
	concatenarInt(buffer, &desplazamiento, idDTB);
	concatenarString(buffer, &desplazamiento, path);
	concatenarInt(buffer, &desplazamiento, error);
	enviarMensaje(socketSAFA, buffer, desplazamiento);
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
				char* datos = obtenerDatosDeMDJ();
				int estadoDeCarga = enviarDatosAFM9(datos);
				if(estadoDeCarga != 0){
					enviarError(idDTB, path, estadoDeCarga);
				}else{
					t_list* direcciones = recibirListaDeDireccionesDeFM9();
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
				int crearArchivo = crearArchivoEnMDJ(path, cantidadDeLineas);
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
		default:
			log_error(logger, "Header desconocido");
	}
}

int identificarse(int emisor, char header){
	if(header == IDENTIFICARSE){
		char identificado = deserializarChar(emisor);
		log_debug(logger, "Handshake de: %c", identificado);
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
	//crear servidor para escuchar al cpu
	direccionServidor direccionDAM = levantarDeConfiguracion(NULL, "PUERTO", ARCHIVO_CONFIGURACION);
	int servidorDAM = crearServidor(direccionDAM.puerto, INADDR_ANY);

	//crear servidores para ser cliente de ellos
	direccionServidor direccionSAFA = levantarDeConfiguracion("IP_SAFA", "PUERTO_SAFA", ARCHIVO_CONFIGURACION);
	socketSAFA = conectarConServidor(direccionSAFA.puerto, inet_addr(direccionSAFA.ip));

	direccionServidor direccionMDJ = levantarDeConfiguracion("IP_MDJ", "PUERTO_MDJ", ARCHIVO_CONFIGURACION);
	socketMDJ = conectarConServidor(direccionMDJ.puerto, inet_addr(direccionMDJ.ip));

	direccionServidor direccionFM9 = levantarDeConfiguracion("IP_FM9", "PUERTO_FM9", ARCHIVO_CONFIGURACION);
	socketFM9 = conectarConServidor(direccionFM9.puerto, inet_addr(direccionFM9.ip));

	//mandar handshakes a los servidores
	handshake(socketSAFA, DAM);
	handshake(socketMDJ, DAM);
	handshake(socketFM9, DAM);

	crearSelect(servidorDAM);

	while(1);
	return 0;
}
