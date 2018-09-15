
#include <biblioteca/socket.h>
#include <biblioteca/hilos.h>
#include <biblioteca/utilidades.h>


void consola(int servidor){
	while(1){
		char* texto = malloc(1000);
		scanf("%s", texto);
		enviarStringSerializado(servidor, texto);
	}
}
void escuchar(int servidor){
	while(1){
		char* buffer=asignarMemoria(150);
		int bytesRecibidos = recibirMensaje(servidor,&buffer,150);
		printf("%s \n", buffer);
		free(buffer);
	}
}

int main(void) {
	direccionServidor direccionSAFA = levantarDeConfiguracion("IP_SAFA", "PUERTO_SAFA", ARCHIVO_CONFIGURACION);
	int socketSAFA = conectarConServidor(direccionSAFA.puerto, inet_addr(direccionSAFA.ip));

	handshake(socketSAFA, CPU);

	pthread_t hiloConsola = crearHilo(&consola,socketSAFA);
	pthread_t hiloEscuchadorSAFA = crearHilo(&escuchar,socketSAFA);

	esperarHilo(hiloConsola);
	esperarHilo(hiloEscuchadorSAFA);

	return 0;
}
