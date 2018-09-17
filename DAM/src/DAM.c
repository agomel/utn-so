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
	voidDeserealizado mensajeAReenviar;
	char identificado;
	char identificarPersonaReenviar;
	u_int32_t personaAReenviar;

	switch(header){
		case IDENTIFICARSE:
			//TODO agregar tambien el socket identificado al mapa de conexiones
			identificado = deserializarIdentificarse(emisor);
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
		case REENVIAR_MENSAJE:
			identificarPersonaReenviar = deserializarIdentificarse(emisor);
			switch(identificarPersonaReenviar){
				case MDJ:
					personaAReenviar = socketMDJ;
					break;
				case FM9:
					personaAReenviar = socketFM9;
					break;
				default:
				perror("no se puede reenviar a esa persona");
			}
			mensajeAReenviar = deserializarVoid(emisor);
			enviarMensaje(personaAReenviar, mensajeAReenviar.mensaje, mensajeAReenviar.tamanioMensaje);
			break;
		default:
			perror("Cualquiera ese header flaco");
	}
}
void escuchar(int servidor){
	int a = 1;
	while(a){
		char* buffer=asignarMemoria(150);
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
