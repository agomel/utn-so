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
void enviarAMDJ(Operacion operacion){
	u_int32_t tamanioBuffer = (strlen(operacion.path)+1) + sizeof(u_int32_t)*3 + sizeof(char);
	void* buffer = asignarMemoria(tamanioBuffer);
	u_int32_t desplazamiento = 0;

	concatenarChar(buffer, &desplazamiento, OBTENER_DATOS);
	concatenarString(buffer, &desplazamiento, operacion.path);
	concatenarInt(buffer, &desplazamiento, operacion.offset);
	concatenarInt(buffer, &desplazamiento, operacion.size);

	enviarMensaje(socketMDJ, buffer, tamanioBuffer);
	free(buffer);
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

void enviarDatosAFM9(char* datos){
	u_int32_t tamanioBuffer = sizeof(char) + sizeof(u_int32_t) + strlen(datos)+1;
	void* buffer = asignarMemoria(tamanioBuffer);
	u_int32_t desplazamiento = 0;

	concatenarChar(buffer, &desplazamiento, GUARDAR_DATOS);
	concatenarString(buffer, &desplazamiento, datos);

	enviarMensaje(socketFM9, buffer, tamanioBuffer);
	free(buffer);
}
void recibirDatosDeFM9YEnviarASafa(Operacion operacion){
	void* buffer;
	u_int32_t desplazamiento = 0;
	u_int32_t estadoDeCarga = deserializarInt(socketFM9);
	u_int32_t tamanioPath = strlen(operacion.path) + 1;
	if(estadoDeCarga == 0){ //No pudo cargar
		buffer = asignarMemoria(sizeof(char) + sizeof(u_int32_t) + sizeof(u_int32_t) + tamanioPath);
		concatenarChar(buffer, &desplazamiento, FALLO_LA_CARGA_DEL_SCRIPTORIO);
		concatenarInt(buffer, &desplazamiento, operacion.idDTB);
		concatenarString(buffer, &desplazamiento, operacion.path);
	}else{
		t_list* listaDirecciones = deserializarListaInt(socketFM9);
		u_int32_t tamanioBuffer = sizeof(char) + sizeof(u_int32_t)
				+ sizeof(u_int32_t) + tamanioPath + sizeof(u_int32_t) + sizeof(u_int32_t)*(listaDirecciones->elements_count);
		concatenarChar(buffer, &desplazamiento, OK_CARGA_DEL_SCRIPTORIO);
		concatenarInt(buffer, &desplazamiento, operacion.idDTB);
		concatenarString(buffer, &desplazamiento, operacion.path);
		concatenarListaInt(buffer, &desplazamiento, listaDirecciones);
	}
	enviarMensaje(socketSAFA, buffer, desplazamiento);
	free(buffer);
}
void* recibirFlushFM9(){
	u_int32_t cantidadTotal = deserializarInt(socketFM9);
	void* memoriaTotal = asignarMemoria(cantidadTotal);
	u_int32_t desplazamiento = 0;
	while(cantidadTotal > 0){
		u_int32_t cantidadARecibir;
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
void enviarDatosAMDJ(void* datosTotales){
	u_int32_t cantidadTotal = sizeof(datosTotales);
	enviarYSerializarInt(socketMDJ, cantidadTotal, GUARDAR_DATOS);
	u_int32_t desplazamiento = 0;
	while(cantidadTotal > 0){
		u_int32_t cantidadAEnviar;
		if(cantidadTotal > transferSize){
			cantidadAEnviar = transferSize;
		}else{
			cantidadAEnviar = cantidadTotal;
		}
		enviarMensaje(socketMDJ, datosTotales + desplazamiento, cantidadAEnviar);
		desplazamiento = desplazamiento + cantidadAEnviar;
		cantidadTotal = cantidadTotal - cantidadAEnviar;
	}
}
void verificarDatosDeMDJYEnviarASafa(Operacion *operacion){
	u_int32_t estadoDeCarga = deserializarInt(socketMDJ);
	if(estadoDeCarga != 1){ //No pudo cargar osea el estado de carga es el numero del error
		enviarError(operacion, estadoDeCarga);
	}else{ //si lo pudo cargar, envio el path donde guardo
		void* buffer = asignarMemoria(sizeof(char) + sizeof(u_int32_t) + strlen(operacion->path)+1);
		u_int32_t desplazamiento = 0;
		concatenarChar(buffer, &desplazamiento, GUARDADO_CON_EXITO_EN_MDJ);
		concatenarInt(buffer, &desplazamiento, operacion->idDTB);
		concatenarString(buffer, &desplazamiento, operacion->path);
		enviarMensaje(socketSAFA, buffer, desplazamiento);
		free(buffer);
	}
}
void enviarError(Operacion* operacion, u_int32_t error){
	void* buffer = asignarMemoria(sizeof(char) + sizeof(u_int32_t) + strlen(operacion->path)+1);
	u_int32_t desplazamiento = 0;
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
		u_int32_t estadoDeOperacion;
		switch(operacion->accion){
			case CARGAR_ESCRIPTORIO_EN_MEMORIA://Dummy
				printf("Ehhh, voy a buscar %s para %d \n", operacion->path, operacion->idDTB);//Esto tendria que ser un log
				enviarAMDJ(*operacion);
				char* datos = deserializarString(socketMDJ);
				enviarDatosAFM9(datos);
				recibirDatosDeFM9YEnviarASafa(*operacion);
				break;
			case GUARDAR_ESCRIPTORIO:
				//TODO enviarAFM9(operacion);// TODO definir que tengo que mandar para que el FM9 lo pueda buscar
				estadoDeOperacion = deserializarInt(socketFM9);
				if(estadoDeOperacion == 0){
					enviarError(operacion, 30002);
				}else{
					void* datosFlush = recibirFlushFM9();
					enviarDatosAMDJ(datosFlush);
					free(datosFlush);
					verificarDatosDeMDJYEnviarASafa(operacion);
				}
				break;
			default:
				perror("Cualquiera ese header flaco");
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
