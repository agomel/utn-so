
#include <biblioteca/socket.h>
#include <biblioteca/hilos.h>
#include <biblioteca/utilidades.h>
#include <commons/config.h>

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
	t_config* configuracion = config_create(ARCHIVO_CONFIGURACION);
	char* ipSAFA = config_get_string_value(configuracion, "IP_SAFA");
	int puertoSAFA = config_get_int_value(configuracion, "PUERTO_SAFA");
	int SAFA = conectarConServidor(puertoSAFA, inet_addr(ipSAFA));

	/*
	 *
	char* puertoSAFA = config_get_string_value(configuracion, "PUERTO_SAFA");

	int SAFA = conectarConServidor("127.0.0.1", inet_addr(ipSAFA));*/
	pthread_t hiloConsola = crearHilo(&consola,SAFA);
	pthread_t hiloEscuchador = crearHilo(&escuchar,SAFA);

	esperarHilo(hiloConsola);
	esperarHilo(hiloEscuchador);

	return 0;
}
