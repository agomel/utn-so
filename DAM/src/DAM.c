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
	free(operacion->offset);
	free(operacion->path);
	free(operacion->size);
	free(operacion);
}

void inicializarDAM(){
	inicializarMutex(&mutexColaOperaciones);
	inicializarSem(&semHayEnColaOperaciones, 0);
	colaOperaciones = queue_create();
	archivoConfig = config_create(ARCHIVO_CONFIGURACION);
	transferSize = config_get_int_value(archivoConfig, "TRANSFER_SIZE");
}
int validarArchivoMDJ(Operacion* operacion){
	void* buffer = asignarMemoria(sizeof(char) + sizeof(u_int32_t) + strlen(operacion->path)+1);
	int desplazamiento = 0;
	concatenarChar(buffer, &desplazamiento, VALIDAR_ARCHIVO);
	concatenarString(buffer, &desplazamiento, operacion->path);
	enviarMensaje(socketMDJ, buffer, desplazamiento);
	free(buffer);
	return deserializarInt(socketMDJ);

}
char* obtenerDatosDeMDJ(Operacion* operacion){
	u_int32_t offset = 0;
	char* datosTotales = asignarMemoria(0);

	u_int32_t cantidadDeDatos = transferSize;
	while(cantidadDeDatos == transferSize){
		void* buffer = asignarMemoria(sizeof(char) + sizeof(u_int32_t) + (strlen(operacion->path)+1)
				+ sizeof(u_int32_t) + sizeof(u_int32_t));
		u_int32_t desplazamiento = 0;

		concatenarChar(buffer, &desplazamiento, OBTENER_DATOS);
		concatenarString(buffer, &desplazamiento, operacion->path);
		concatenarInt(buffer, &desplazamiento, offset);
		concatenarInt(buffer, &desplazamiento, transferSize);

		enviarMensaje(socketMDJ, buffer, desplazamiento);
		free(buffer);

		cantidadDeDatos = deserializarInt(socketMDJ);
		char* datosParciales = asignarMemoria(cantidadDeDatos);

		datosTotales = realloc(datosTotales, sizeof(datosTotales) + cantidadDeDatos);
		concatenarString(datosTotales, offset, datosParciales);

		offset = offset + cantidadDeDatos;
	}
	return datosTotales;
}
void agregarOperacionACola(int emisor, char accion){
	Operacion* operacion = asignarMemoria(sizeof(Operacion));
	operacion->path = deserializarString(emisor);
	operacion->idDTB = deserializarInt(emisor);
	operacion->offset = deserializarInt(emisor);
	operacion->size = deserializarInt(emisor);
	operacion->accion = accion;

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
	void* buffer = asignarMemoria(sizeof(char) + sizeof(u_int32_t) + (strlen(operacion->path)+1) + sizeof(u_int32_t));
	int desplazamiento = 0;
	concatenarChar(buffer, &desplazamiento, CREAR_ARCHIVO);
	concatenarString(buffer, &desplazamiento, operacion->path);
	concatenarInt(buffer, &desplazamiento, cantidadDeLineas);
	enviarMensaje(socketMDJ, buffer, desplazamiento);
	free(buffer);
	return deserializarInt(socketMDJ);
}
void* recibirFlushFM9(){
	int cantidadTotal = deserializarInt(socketFM9);
	void* memoriaTotal = asignarMemoria(cantidadTotal);
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
	concatenarChar(buffer, &desplazamiento, GUARDADO_CON_EXITO);
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
				printf("Ehhh, voy a buscar %s para %d \n", operacion->path, operacion->idDTB);//Esto tendria que ser un log
				u_int32_t validarArchivo = validarArchivoMDJ(operacion);
				if(validarArchivo != 0){
					enviarError(operacion, validarArchivo);
				}else {
					char* datos = obtenerDatosDeMDJ(operacion);
					u_int32_t estadoDeCarga = enviarDatosAFM9(datos);
					if(estadoDeCarga != 0){
						enviarError(operacion, estadoDeCarga);
					}else{
						notificarASafa(operacion);
					}
				}
				break;
			case GUARDAR_ESCRIPTORIO:
				//TODO definir que tengo que mandar para que el FM9 lo pueda buscar
				//TODO estadoDeOperacion = enviarAFM9(operacion);
				// le pido a FM9 los datos y espero la respuesta a ver si hay un error
				printf("mierda");
				int estadoDeOperacion = 0;
				if(estadoDeOperacion != 0){
					enviarError(operacion, estadoDeOperacion);
				}else{
					//TODO
					u_int32_t cantidadDeLineas = deserializarInt(socketFM9);
					//hay que hacer esta funcionalidad, que reciba la cantidad de lineas a guardar
					u_int32_t crearArchivo = crearArchivoEnMDJ(operacion, cantidadDeLineas);
					if(crearArchivo != 0){
						enviarError(operacion, crearArchivo);
					}else{
						void* datosFlush = recibirFlushFM9();
						u_int32_t guardarDatos = guardarDatosEnMDJ(datosFlush);
						free(datosFlush);
						if(guardarDatos != 0){
							enviarError(operacion, guardarDatos);
						}else{
							notificarASafa(operacion);
						}
					}
				}
				break;
			default:
				printf("no");
				//log_error(logger, "Header desconocido");
			}
		freeOperacion(operacion);
	}
}

void escucharCPU(int socketCPU){
	printf("Escuchando a cpu \n");
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
