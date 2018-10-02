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
	inicializarSem(semHayEnColaOperaciones, 0);
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
void recibirDatosDeFM9(void* buffer, u_int32_t* desplazamiento){
	u_int32_t estadoDeCarga = deserializarInt(socketFM9);

	if(estadoDeCarga == 0){ //No pudo cargar
		buffer = asignarMemoria(10);
		//esta parte esta en el branch exitPCD

	}else{//Pudo cargar los datos ok
		//TODO que pasa si esta ok lo que guardo el MDJ ?
	}
}

void escuchar(int servidor){
	int a = 1;
	while(a){
		char* buffer = asignarMemoria(150);
		int bytesRecibidos = recibirMensaje(servidor,&buffer,150);
		if(bytesRecibidos <= 0){
			a = 0;
		}
		printf("%s \n", buffer);
		free(buffer);
	}
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
				enviarAMDJ(operacion);
				char* datos = deserializarString(socketMDJ);
				enviarDatosAFM9(datos);
				void* buffer;
				u_int32_t desplazamiento;
				recibirDatosDeFM9(buffer, &desplazamiento);
				//TODO enviar y serializar estructura
				free(buffer);
			break;
			default:
				perror("Cualquiera ese header flaco");
			}
	}
}

void escucharCPU(int socketCPU){
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

	parametrosEscucharClientes parametros;
	parametros.servidor = servidorDAM;
	parametros.funcion = &entenderMensaje;

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

	//TODO crear hilo de la otra forma
	crearHilo(&consumirCola, NULL);

	while(1){
		int cpu = aceptarCliente(servidorDAM);
		//TODO crear hilo de la otra forma
		crearHilo(&escucharCPU, cpu);
	}
	return 0;
}
