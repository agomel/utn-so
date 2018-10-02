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
		//TODO que pasa si esta ok lo que guardo el MDJ ?
		//deserializar lista int
		//TODO hacer lo  que pasa si vuelve bien
		//TODO deserializar lista y ver que onda
		//concatenarChar(buffer, &desplazamiento, PASAR_READY);
		//concatenarChar(buffer, &desplazamiento, COLA_NEW); //de donde sacar el proceso
		//idDtb = 0; //TODO poner de donde lo saca?!?!
		//concatenarInt(buffer, &desplazamiento, idDtb);
		//TODO concatenar lista de tabla de paginas
	}
	enviarMensaje(socketSAFA, buffer, desplazamiento);
}

void consumirCola(){
	while(1){
		waitSem(&semHayEnColaOperaciones);
		waitMutex(&mutexColaOperaciones);
		Operacion* operacion = queue_pop(colaOperaciones);
		signalMutex(&mutexColaOperaciones);

		switch(operacion->accion){
			case CARGAR_ESCRIPTORIO://Dummy
				//aca tengo que pedirle al mdj esperar etc...
				enviarAMDJ(*operacion);
				char* datos = deserializarString(socketMDJ);
				enviarDatosAFM9(datos);
				recibirDatosDeFM9YEnviarASafa(operacion->idDTB);
				free(buffer);
			break;
			default:
				perror("Cualquiera ese header flaco");
			}
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
	direccionServidor direccionDAM = levantarDeConfiguracion(NULL, "PUERTO", ARCHIVO_CONFIGURACION, archivoConfiguracion);
	int servidorDAM = crearServidor(direccionDAM.puerto, INADDR_ANY);

	//crear servidores para ser cliente de ellos
	direccionServidor direccionSAFA = levantarDeConfiguracion("IP_SAFA", "PUERTO_SAFA", ARCHIVO_CONFIGURACION, archivoConfiguracion);
	socketSAFA = conectarConServidor(direccionSAFA.puerto, inet_addr(direccionSAFA.ip));

	direccionServidor direccionMDJ = levantarDeConfiguracion("IP_MDJ", "PUERTO_MDJ", ARCHIVO_CONFIGURACION, archivoConfiguracion);
	socketMDJ = conectarConServidor(direccionMDJ.puerto, inet_addr(direccionMDJ.ip));

	direccionServidor direccionFM9 = levantarDeConfiguracion("IP_FM9", "PUERTO_FM9", ARCHIVO_CONFIGURACION, archivoConfiguracion);
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
