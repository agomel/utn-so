
#include <biblioteca/socket.h>
#include <biblioteca/hilos.h>
#include <biblioteca/utilidades.h>
#include <biblioteca/dtb.h>
int socketDIEGO;
int socketFM9;
int socketSAFA;
void consola(int servidor){
	while(1){
		char* texto = malloc(1000);
		scanf("%s", texto);
		enviarYSerializarString(servidor, texto, MANDAR_TEXTO);
	}
}
void escuchar(int servidor){
	int a = 1;
	u_int32_t tamanioPathEscriptorio;
	u_int32_t tamanioBuffer;
	void* buffer;
	u_int32_t desplazamiento;
	u_int32_t sizeDelEscriptorio = 10;

	while(a){
		DTB dtbRecibido;
		char header = deserializarChar(servidor);
			switch(header){
				case ENVIAR_DTB:
					printf("llego un dtb\n");
					dtbRecibido = deserializarDTB(servidor);
					if(dtbRecibido.flag == 0){
						//Es el dummy
						tamanioPathEscriptorio = strlen(dtbRecibido.escriptorio) + 1;
						tamanioBuffer = sizeof(char) + tamanioPathEscriptorio + sizeof(u_int32_t)*4;
						buffer = asignarMemoria(tamanioBuffer);
						desplazamiento = 0;

						concatenarChar(buffer, &desplazamiento, CARGAR_ESCRIPTORIO_EN_MEMORIA);
						concatenarString(buffer, &desplazamiento, dtbRecibido.escriptorio);
						concatenarInt(buffer, &desplazamiento, dtbRecibido.id);
						concatenarInt(buffer, &desplazamiento, 0); //Offset (Desde el principio)
						concatenarInt(buffer, &desplazamiento, -1); //size  (Hasta el final)

						enviarMensaje(socketDIEGO, buffer, tamanioBuffer);
						free(buffer);
					}else{
						//No es el dummy
						printf("Ejecutar dtb");
					}
					break;
		default:
					perror("Cualquiera ese header flaco");
			}
	}
}

int main(void) {
	direccionServidor direccionSAFA = levantarDeConfiguracion("IP_SAFA", "PUERTO_SAFA", ARCHIVO_CONFIGURACION);
	socketSAFA = conectarConServidor(direccionSAFA.puerto, inet_addr(direccionSAFA.ip));
	handshake(socketSAFA, CPU);
	printf("Socket del safa %d \n", socketSAFA);

	direccionServidor direccionFM9 = levantarDeConfiguracion("IP_FM9", "PUERTO_FM9", ARCHIVO_CONFIGURACION);
	socketFM9 = conectarConServidor(direccionFM9.puerto, inet_addr(direccionFM9.ip));
	handshake(socketFM9, CPU);

	direccionServidor direccionDIEGO = levantarDeConfiguracion("IP_DIEGO", "PUERTO_DIEGO", ARCHIVO_CONFIGURACION);
	socketDIEGO= conectarConServidor(direccionDIEGO.puerto, inet_addr(direccionDIEGO.ip));

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
