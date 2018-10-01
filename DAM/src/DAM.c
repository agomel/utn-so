/*
 ============================================================================
 Name        : DAM.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include <biblioteca/socket.h>
#include <biblioteca/hilos.h>
#include <biblioteca/utilidades.h>
#include <biblioteca/select.h>
#include <biblioteca/serializacion.h>
#include <biblioteca/dtb.h>

u_int32_t socketCPU;
u_int32_t socketFM9;
u_int32_t socketMDJ;
u_int32_t socketSAFA;

void entenderMensaje(int emisor, char header){
	char identificado;
	char identificarPersonaReenviar;
	u_int32_t personaAReenviar;
	char* datos;
	u_int32_t idDtb;
	u_int32_t estadoDeCarga;
	void* buffer;
	u_int32_t desplazamiento;
	u_int32_t tamanioPath;
	char* path;
	u_int32_t tamanioBuffer;
	u_int32_t offset;
	u_int32_t sizeDelEscriptorio;
	u_int32_t idDTBNuevo;

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
			//TODO fijarse el transfer size porque no puede cargar todo de una.
			path = deserializarString(emisor);
			idDTBNuevo = deserializarInt(emisor);
			tamanioBuffer = strlen(path) + sizeof(u_int32_t)*3 + sizeof(char);
			buffer = asignarMemoria(tamanioBuffer);
			desplazamiento = 0;
			offset = 0; //Quiero que lea el archivo desde el principio
			sizeDelEscriptorio = 100; //Cómo sabemos el tamaño de lo que va a traer?!?!?

			concatenarChar(buffer, &desplazamiento, OBTENER_DATOS);
			concatenarString(buffer, &desplazamiento, path);
			concatenarInt(buffer, &desplazamiento, offset);
			concatenarInt(buffer, &desplazamiento, sizeDelEscriptorio);
			enviarMensaje(socketMDJ, buffer, tamanioBuffer);
			free(buffer);
			break;

		case DATOS_CONSEGUIDOS:
			datos = deserializarString(emisor);
			enviarYSerializarString(socketFM9, datos, GUARDAR_DATOS);
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

	//creo un hilo para escuchar los clientes(CPU)
	pthread_t hiloAdministradorDeConexiones = crearHilo(&escucharClientes, &parametros);

	//esperar hilos
	esperarHilo(hiloEscuchadorSAFA);
	esperarHilo(hiloEscuchadorMDJ);
	esperarHilo(hiloEscuchadorFM9);
	esperarHilo(hiloAdministradorDeConexiones);

	return 0;
}
