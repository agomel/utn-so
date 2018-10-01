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
	inicializarMutex(&mutexColaMDJ);
	inicializarMutex(&mutexMDJDisponible);
	inicializarSem(&semHayEnColaMDJ,0);
}

void enviarAMDJ(){
	waitSem(&semHayEnColaMDJ);

	waitMutex(&mutexColaMDJ);
	ProcesoMDJ* procesoMDJ = queue_peek(colaMDJ);
	signalMutex(&mutexColaMDJ);

	u_int32_t tamanioBuffer = (strlen(procesoMDJ->path)+1) + sizeof(u_int32_t)*3 + sizeof(char);
	void* buffer = asignarMemoria(tamanioBuffer);
	u_int32_t desplazamiento = 0;

	concatenarChar(buffer, &desplazamiento, OBTENER_DATOS);
	concatenarString(buffer, &desplazamiento, procesoMDJ->path);
	concatenarInt(buffer, &desplazamiento, procesoMDJ->offset);
	concatenarInt(buffer, &desplazamiento, procesoMDJ->size);

	waitMutex(&mutexMDJDisponible);
	enviarMensaje(socketMDJ, buffer, tamanioBuffer);
	free(buffer);
}

void agregarDTBAColaMDJ(int emisor, int esDummy){
	ProcesoMDJ* dtb = asignarMemoria(sizeof(ProcesoMDJ));
	dtb->path = deserializarString(emisor);
	dtb->idDTB = deserializarInt(emisor);
	if(esDummy){
		dtb->offset = 0;
		//TODO el MDJ tiene que saber que si el size es -1 tiene que leer hasta el final del archivo
		dtb->size = -1;
	}else{
		dtb->offset = deserializarInt(emisor);
		dtb->size = deserializarInt(emisor);
	}
	waitMutex(&mutexColaMDJ);
	queue_push(colaMDJ, dtb);
	signalMutex(&mutexColaMDJ);
	signalSem(&semHayEnColaMDJ);
}
void enviarDatosAFM9(int emisor){
	signalMutex(&mutexMDJDisponible);

	waitMutex(&mutexColaMDJ);
	ProcesoMDJ* dtb = queue_pop(colaMDJ);
	signalMutex(&mutexColaMDJ);

	//TODO recibir los datos del mdj y crear lo que enviar
	char* datos = deserializarString(emisor);

	u_int32_t tamanioBuffer = strlen(datos)+1 + sizeof(u_int32_t)*2 + sizeof(char);
	void* buffer = asignarMemoria(tamanioBuffer);
	u_int32_t desplazamiento = 0;

	concatenarChar(buffer, &desplazamiento, GUARDAR_DATOS);
	concatenarString(buffer, &desplazamiento, datos);
	concatenarInt(buffer, &desplazamiento, dtb->idDTB);

	enviarMensaje(socketFM9, buffer, tamanioBuffer);
	free(buffer);
	free(dtb);

}

void entenderMensaje(int emisor, char header){
	char identificado;
	char identificarPersonaReenviar;
	u_int32_t personaAReenviar;
	char* datos;
	u_int32_t estadoDeCarga;
	u_int32_t desplazamiento;
	u_int32_t idDtb;
	void* buffer;

	switch(header){
		case IDENTIFICARSE:
			identificado = deserializarChar(emisor);
			printf("identificado %c \n", identificado);
			switch(identificado){
				case CPU:
					socketCPU = emisor;
					break;
				default:
					perror("no acepto a esta conexion");
			}
			printf("Se agrego a las conexiones %c \n" , identificado);
			break;

		case MANDAR_TEXTO:
			//TODO esta operacion es basura, es para probar a serializacion y deserializacion
			deserializarString(emisor);
			break;

		case CARGAR_ESCRIPTORIO:
			//Se le envia 1 como segundo parametro porque el dtb a agregar a la cola es el dummy
			agregarDTBAColaMDJ(emisor, 1);
			break;

		case DATOS_CONSEGUIDOS:
			enviarDatosAFM9(emisor);
			break;

		case RESPUESTA_CARGA: //Si el FM9 pudo cargar bien los datos o no
			estadoDeCarga = deserializarInt(emisor);
			buffer = asignarMemoria(sizeof(u_int32_t) + sizeof(char)*2);
			desplazamiento = 0;
			if(estadoDeCarga == 0){ //No pudo cargar
				concatenarChar(buffer, &desplazamiento, PASAR_EXIT);
			}else{
				concatenarChar(buffer, &desplazamiento, PASAR_READY);
			}
			concatenarChar(buffer, &desplazamiento, COLA_NEW); //de donde sacar el proceso
			idDtb = 0; //TODO poner de donde lo saca?!?!
			concatenarInt(buffer, &desplazamiento, idDtb);
			//TODO concatenar lista de tabla de paginas
			enviarMensaje(socketSAFA, buffer, sizeof(char)*2);
			free(buffer);
			break;

		default:
			perror("Cualquiera ese header flaco");
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

	//creo un hilo para cada servidor con el que me conecto
	pthread_t hiloEscuchadorSAFA = crearHilo(&escuchar,socketSAFA);
	pthread_t hiloEscuchadorMDJ = crearHilo(&escuchar,socketMDJ);
	pthread_t hiloEscuchadorFM9 = crearHilo(&escuchar,socketFM9);
	pthread_t hiloControladorDeMDJ = crearHilo(&enviarAMDJ,hiloControladorDeMDJ);
	//creo un hilo para escuchar los clientes(CPU)
	pthread_t hiloAdministradorDeConexiones = crearHilo(&escucharClientes, &parametros);

	//esperar hilos
	esperarHilo(hiloEscuchadorSAFA);
	esperarHilo(hiloEscuchadorMDJ);
	esperarHilo(hiloEscuchadorFM9);
	esperarHilo(hiloAdministradorDeConexiones);
	esperarHilo(hiloControladorDeMDJ);

	return 0;
}
