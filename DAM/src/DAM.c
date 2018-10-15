/*
 ============================================================================
 Name        : DAM.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include "DAM.h"

void freeOperacion(Operacion* operacion){
	free(operacion->accion);
	free(operacion->idDTB);
	free(operacion->path);
	//TODO hacer el free de la lista de direcciones
	free(operacion);
}

void inicializarDAM(){
	inicializarMutex(&mutexColaOperaciones);
	inicializarSem(&semHayEnColaOperaciones, 0);
	colaOperaciones = queue_create();
	configuraciones = config_create(ARCHIVO_CONFIGURACION);
	transferSize = config_get_int_value(configuraciones, "TRANSFER_SIZE");
	logger = crearLogger(ARCHIVO_LOG, "DAM");
}
int validarArchivoMDJ(Operacion* operacion){
	int tamanioMensaje = sizeof(char) + sizeof(int) + strlen(operacion->path)+1;
	void* buffer = asignarMemoria(tamanioMensaje);
	int desplazamiento = 0;
	concatenarChar(buffer, &desplazamiento, VALIDAR_ARCHIVO);
	concatenarString(buffer, &desplazamiento, operacion->path);
	log_info(logger, "enviando a emisor %d char: %c y escriptorio %s", socketMDJ, VALIDAR_ARCHIVO, operacion->path);
	enviarMensaje(socketMDJ, buffer, tamanioMensaje);
	free(buffer);
	int respuesta =  deserializarInt(socketMDJ);
	return respuesta;

}
char* obtenerDatosDeMDJ(Operacion* operacion){
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
void agregarOperacionACola(int emisor, char accion){
	Operacion* operacion = asignarMemoria(sizeof(Operacion));
	operacion->path = deserializarString(emisor);
	operacion->idDTB = deserializarInt(emisor);
	operacion->accion = accion;
	if(accion == GUARDAR_ESCRIPTORIO)
		operacion->direcciones = deserializarListaInt(emisor);

	waitMutex(&mutexColaOperaciones);
	queue_push(colaOperaciones, operacion);
	signalMutex(&mutexColaOperaciones);
	signalSem(&semHayEnColaOperaciones);
}

int enviarDatosAFM9(char* datos){
	int tamanioBuffer = sizeof(char) + sizeof(int) + strlen(datos)+1;
	void* buffer = asignarMemoria(tamanioBuffer);
	int desplazamiento = 0;

	concatenarChar(buffer, &desplazamiento, GUARDAR_DATOS);
	concatenarString(buffer, &desplazamiento, datos);

	enviarMensaje(socketFM9, buffer, tamanioBuffer);
	free(buffer);

	return deserializarInt(socketFM9);
}
int crearArchivoEnMDJ(Operacion* operacion, int cantidadDeLineas){
	void* buffer = asignarMemoria(sizeof(char) + sizeof(int) + (strlen(operacion->path)+1) + sizeof(int));
	int desplazamiento = 0;
	concatenarChar(buffer, &desplazamiento, CREAR_ARCHIVO);
	concatenarString(buffer, &desplazamiento, operacion->path);
	concatenarInt(buffer, &desplazamiento, cantidadDeLineas);
	enviarMensaje(socketMDJ, buffer, desplazamiento);
	free(buffer);
	return deserializarInt(socketMDJ);
}
int pedirDatosAFM9(Operacion* operacion){
	void* buffer = asignarMemoria(sizeof(char) + sizeof(int) + (sizeof(int)*operacion->direcciones->elements_count));
	int desplazamiento = 0;

	concatenarChar(buffer, &desplazamiento, OBTENER_DATOS);
	concatenarListaInt(buffer, &desplazamiento, operacion->direcciones);
	enviarMensaje(socketFM9, buffer, desplazamiento);

	return deserializarInt(socketFM9);
}
char* recibirFlushFM9(){
	int cantidadTotal = deserializarInt(socketFM9);
	char* memoriaTotal = asignarMemoria(cantidadTotal);
	int desplazamiento = 0;
	while(cantidadTotal > 0){
		int cantidadARecibir;
		if(cantidadTotal > transferSize){
			cantidadARecibir = transferSize;
		}else{
			cantidadARecibir = cantidadTotal;
		}
		void* buffer = asignarMemoria(cantidadARecibir);
		recibirMensaje(socketFM9, buffer, cantidadARecibir);
		concatenarVoid(memoriaTotal, &desplazamiento, buffer, cantidadARecibir);
		free(buffer);
		cantidadTotal = cantidadTotal - cantidadARecibir;
	}
	return memoriaTotal;
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
void notificarASafa(Operacion *operacion){
	void* buffer = asignarMemoria(sizeof(char) + sizeof(int) + strlen(operacion->path)+1);
	int desplazamiento = 0;
	concatenarChar(buffer, &desplazamiento, CARGADO_CON_EXITO_EN_MEMORIA);
	concatenarInt(buffer, &desplazamiento, operacion->idDTB);
	concatenarString(buffer, &desplazamiento, operacion->path);
	enviarMensaje(socketSAFA, buffer, desplazamiento);
	free(buffer);
}
void enviarError(Operacion* operacion, int error){
	void* buffer = asignarMemoria(sizeof(char) + sizeof(int) + strlen(operacion->path)+1);
	int desplazamiento = 0;
	concatenarChar(buffer, &desplazamiento, ERROR);
	concatenarInt(buffer, &desplazamiento, operacion->idDTB);
	concatenarString(buffer, &desplazamiento, operacion->path);
	concatenarInt(buffer, &desplazamiento, error);
	enviarMensaje(socketSAFA, buffer, desplazamiento);
}

void consumirCola(){
	while(1){
		waitSem(&semHayEnColaOperaciones);
		waitMutex(&mutexColaOperaciones);
		Operacion* operacion = queue_pop(colaOperaciones);
		signalMutex(&mutexColaOperaciones);

		switch(operacion->accion){
			case CARGAR_ESCRIPTORIO_EN_MEMORIA:
				log_info(logger, "Ehhh, voy a buscar %s para %d", operacion->path, operacion->idDTB);
				int validarArchivo = validarArchivoMDJ(operacion);
				if(validarArchivo != 0){
					enviarError(operacion, validarArchivo);
				}else {
					char* datos = obtenerDatosDeMDJ(operacion);
					int estadoDeCarga = enviarDatosAFM9(datos);
					if(estadoDeCarga != 0){
						enviarError(operacion, estadoDeCarga);
					}else{
						notificarASafa(operacion);
					}
				}
				break;
			case GUARDAR_ESCRIPTORIO:
				log_debug(logger, "Guardando escriptorio");
				int estadoDeOperacion = pedirDatosAFM9(operacion);
				if(estadoDeOperacion != 0){
					enviarError(operacion, estadoDeOperacion);
				}else{
					int cantidadDeLineas = deserializarInt(socketFM9);
					void* datos = recibirFlushFM9();
					int crearArchivo = crearArchivoEnMDJ(operacion, cantidadDeLineas);
					if(crearArchivo != 0){
						enviarError(operacion, crearArchivo);
					}else{
						int guardarDatos = guardarDatosEnMDJ(datos);
						if(guardarDatos != 0){
							enviarError(operacion, guardarDatos);
						}else{
							notificarASafa(operacion);
						}
					}
					free(datos);
				}
				break;
			default:
				log_error(logger, "Header desconocido");
			}
		freeOperacion(operacion);
	}
}

void escucharCPU(int socketCPU){
	log_debug(logger, "Escuchando a cpu");
	while(1){
		char header;
		recibirMensaje(socketCPU, &header, sizeof(char));
		agregarOperacionACola(socketCPU, header);
		//esto solo agrega operaciones a la cola
	}
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

	crearHiloQueMuereSolo(&consumirCola, NULL);
	empezarAEscuchar(servidorDAM, 100);
	while(1){
		int cpu = aceptarCliente(servidorDAM);
		crearHiloQueMuereSolo(&escucharCPU, cpu);
	}
	return 0;
}
