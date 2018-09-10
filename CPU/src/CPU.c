
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

typedef struct{
	char* ip;
	int puerto;
}direccionServidor;

direccionServidor levantarDeConfiguracion(char* nombreIp, char* nombrePuerto, char* rutaArchivo){
	t_config* configuracion = config_create(rutaArchivo);
	direccionServidor direccion;
	direccion.ip = config_get_string_value(configuracion, nombreIp);
	direccion.puerto = config_get_int_value(configuracion, nombrePuerto);
	return direccion;
}

int main(void) {
	direccionServidor direccionSAFA = levantarDeConfiguracion("IP_SAFA", "PUERTO_SAFA", ARCHIVO_CONFIGURACION);
	int SAFA = conectarConServidor(direccionSAFA.puerto, inet_addr(direccionSAFA.ip));
	enviarIdentificacion("cpu", SAFA);

	/*direccionServidor direccionDAM = levantarDeConfiguracion("IP_DIEGO", "PUERTO_DIEGO", ARCHIVO_CONFIGURACION);
	int DAM = conectarConServidor(direccionDAM.puerto, inet_addr(direccionDAM.ip));
	enviarIdentificacion("cpu",DAM);

	direccionServidor direccionFM9 = levantarDeConfiguracion("IP_FM9", "PUERTO_FM9", ARCHIVO_CONFIGURACION);
	int FM9 = conectarConServidor(direccionFM9.puerto, inet_addr(direccionFM9.ip));
	enviarIdentificacion("cpu", FM9);*/


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
