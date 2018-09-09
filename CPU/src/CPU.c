
#include <biblioteca/socket.h>

void consola(int servidor){
	while(1){
		char mensaje[1000];
		scanf("%s", mensaje);
		enviarMensaje(servidor, mensaje);
	}
}
void escuchar(int servidor){
	while(1){
		char* buffer=asignarMemoria(150);
		int bytesRecibidos = recibirMensaje(servidor,&buffer,149);
		printf("%s \n", buffer);
		free(buffer);
	}
}
int main(void) {
	int SAFA = conectarConServidor(20001, inet_addr("127.0.0.1"));

	pthread_t hiloConsola = crearHilo(&consola,SAFA);
	pthread_t hiloEscuchador = crearHilo(&escuchar,SAFA);

	esperarHilo(hiloConsola);
	esperarHilo(hiloEscuchador);

	return 0;
}
