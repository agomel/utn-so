
#include <biblioteca/socket.h>
#include <biblioteca/hilos.h>
#include <biblioteca/utilidades.h>
#include <biblioteca/dtb.h>

void entenderMensaje(int emisor, char header){
	DTB dtbRecibido;
	char identificado;
	switch(header){
		case ENVIAR_DTB:
			dtbRecibido = deserializarDTB(emisor);
			if(dtbRecibido.flag == 1){
				u_int32_t tamanioEscriptorio = strlen(dtbRecibido.escriptorio) + 1;
				u_int32_t tamanioBuffer = sizeof(u_int32_t) + sizeof(char)*3 + tamanioEscriptorio;
				void* buffer = asignarMemoria(tamanioBuffer);

				//Lleno el buffer
				u_int32_t desplazamiento = 0;

				concatenarChar(buffer, &desplazamiento, REENVIAR_MENSAJE);
				concatenarChar(buffer, &desplazamiento, MDJ);
				concatenarInt(buffer, &desplazamiento, sizeof(u_int32_t) + sizeof(char) + tamanioEscriptorio);
				concatenarChar(buffer, &desplazamiento, OBTENER_DATOS);
				concatenarString(buffer, &desplazamiento, dtbRecibido.escriptorio);

				enviarMensaje(emisor, buffer, tamanioBuffer);

				free(buffer);
			}
/*			switch(identificado){
				case CPU:
					socketCPU = emisor;
					break;
				case DAM:
					socketDAM = emisor;
					break;
				default:
					perror("no acepto a esta conexion");
			}
			printf("Se agrego a las conexiones %c \n" , identificado);
			break;*/
		default:
			perror("Cualquiera ese header flaco");
	}
}

void consola(int servidor){
	while(1){
		char* texto = malloc(1000);
		scanf("%s", texto);
		enviarYSerializarString(servidor, texto,MANDAR_TEXTO);
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
	direccionServidor direccionSAFA = levantarDeConfiguracion("IP_SAFA", "PUERTO_SAFA", ARCHIVO_CONFIGURACION);
	int socketSAFA = conectarConServidor(direccionSAFA.puerto, inet_addr(direccionSAFA.ip));
	handshake(socketSAFA, CPU);

	direccionServidor direccionFM9 = levantarDeConfiguracion("IP_FM9", "PUERTO_FM9", ARCHIVO_CONFIGURACION);
	int socketFM9 = conectarConServidor(direccionFM9.puerto, inet_addr(direccionFM9.ip));
	handshake(socketFM9, CPU);

	direccionServidor direccionDIEGO = levantarDeConfiguracion("IP_DIEGO", "PUERTO_DIEGO", ARCHIVO_CONFIGURACION);
	int socketDIEGO= conectarConServidor(direccionDIEGO.puerto, inet_addr(direccionDIEGO.ip));
	handshake(socketDIEGO, CPU);

	pthread_t hiloConsola = crearHilo(&consola, socketSAFA);
	pthread_t hiloEscuchadorSAFA = crearHilo(&escuchar, socketSAFA);
	pthread_t hiloEscuchadorFM9= crearHilo(&escuchar, socketFM9);
	pthread_t hiloEscuchadorDIEGO = crearHilo(&escuchar, socketDIEGO);

	esperarHilo(hiloConsola);
	esperarHilo(hiloEscuchadorSAFA);
	esperarHilo(hiloEscuchadorFM9);
	esperarHilo(hiloEscuchadorDIEGO);
	return 0;
}
