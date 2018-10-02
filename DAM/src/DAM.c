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

void inicializarDAM(){
	inicializarMutex(&mutexColaOperaciones);
	inicializarSem(&semHayEnColaOperaciones, 0);
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
	free(operacion);
	signalSem(&semHayEnColaOperaciones);
}

void enviarDatosAFM9(char* datos){
	u_int32_t tamanioBuffer = strlen(datos)+1 + sizeof(u_int32_t) + sizeof(char);
	void* buffer = asignarMemoria(tamanioBuffer);
	u_int32_t desplazamiento = 0;

	concatenarChar(buffer, &desplazamiento, GUARDAR_DATOS);
	concatenarString(buffer, &desplazamiento, datos);

	enviarMensaje(socketFM9, buffer, tamanioBuffer);
	free(buffer);
}
void recibirDatosDeFM9YEnviarASafa(u_int32_t idDTB){
	void* buffer;
	u_int32_t desplazamiento = 0;
	u_int32_t estadoDeCarga = deserializarInt(socketFM9);
	if(estadoDeCarga == 0){ //No pudo cargar
		buffer = asignarMemoria(sizeof(char) + sizeof(u_int32_t));
		concatenarChar(buffer, &desplazamiento, FALLO_LA_CARGA_DEL_SCRIPTORIO);
		concatenarInt(buffer, &desplazamiento, idDTB);
	}else{
		t_list* listaDirecciones = deserializarListaInt(socketFM9);
		u_int32_t tamanioBuffer = sizeof(char)*2 + sizeof(u_int32_t) + sizeof(u_int32_t) + sizeof(u_int32_t)*(listaDirecciones->elements_count);
		concatenarChar(buffer, &desplazamiento, PASAR_READY);
		concatenarChar(buffer, &desplazamiento, COLA_NEW);
		concatenarInt(buffer, &desplazamiento, idDTB);
		concatenarListaInt(buffer, &desplazamiento, listaDirecciones);
	}
	enviarMensaje(socketSAFA, buffer, desplazamiento);
	free(buffer);
}

void consumirCola(){
	while(1){
		Operacion* operacion = asignarMemoria(sizeof(Operacion));
		waitSem(&semHayEnColaOperaciones);
		waitMutex(&mutexColaOperaciones);
		operacion = queue_pop(colaOperaciones);
		signalMutex(&mutexColaOperaciones);

		switch(operacion->accion){
			case CARGAR_ESCRIPTORIO://Dummy
				enviarAMDJ(*operacion);
				char* datos = deserializarString(socketMDJ);
				enviarDatosAFM9(datos);
				recibirDatosDeFM9YEnviarASafa(operacion->idDTB);
				break;
			default:
				perror("Cualquiera ese header flaco");
			}
		free(operacion);
	}
}

void escucharCPU(int socketCPU){
	printf("Escuchando a cpu \n");
	while(1){
		char header;
		recibirMensaje(socketCPU, &header, sizeof(char));
		agregarOperacionACola(socketCPU, header);
		//esto solo agrega operaciones ala colas
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
