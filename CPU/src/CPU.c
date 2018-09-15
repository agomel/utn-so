
#include <biblioteca/socket.h>
#include <biblioteca/hilos.h>
#include <biblioteca/utilidades.h>


void consola(int servidor){
	while(1){
		char mensaje[1000];
		scanf("%s", mensaje);
		enviarMensaje(servidor, mensaje, strlen(mensaje));
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
	int SAFA = conectarConServidor(direccionSAFA.puerto, inet_addr(direccionSAFA.ip));
	int tamanioMensaje = 10;
	char* handshake = malloc(tamanioMensaje);
	char op = IDENTIFICARSE;
	char* cpu = "hola";

	int puntero = 0;
	memcpy(handshake + puntero, &op, sizeof(op));

	puntero = puntero + sizeof(op);
	u_int32_t tam = strlen(cpu)+1;
	memcpy(handshake + puntero, &tam, sizeof(u_int32_t));

	puntero = puntero + sizeof(u_int32_t);
	memcpy(handshake + puntero, cpu, tam);
	//puntero = puntero + strlen(cpu);

	enviarMensaje(SAFA, handshake, tamanioMensaje);

	free(handshake);
	//char* mensajeSerializado=serializarMensaje(IDENTIFICARSE,"CPU");
	//enviarMensaje(SAFA,mensajeSerializado);
	//enviarIdentificacion("cpu", SAFA);

	//direccionServidor direccionFM9 = levantarDeConfiguracion("IP_FM9", "PUERTO_FM9", ARCHIVO_CONFIGURACION);
	//int FM9 = conectarConServidor(direccionFM9.puerto, inet_addr(direccionFM9.ip));
	//enviarIdentificacion("cpu", FM9);

	/*direccionServidor direccionDAM = levantarDeConfiguracion("IP_DIEGO", "PUERTO_DIEGO", ARCHIVO_CONFIGURACION);
	int DAM = conectarConServidor(direccionDAM.puerto, inet_addr(direccionDAM.ip));
	enviarIdentificacion("cpu",DAM);

	*/


	/*
	 *
	char* puertoSAFA = config_get_string_value(configuracion, "PUERTO_SAFA");

	int SAFA = conectarConServidor("127.0.0.1", inet_addr(ipSAFA));*/

	pthread_t hiloConsola = crearHilo(&consola,SAFA);
	pthread_t hiloEscuchadorSAFA = crearHilo(&escuchar,SAFA);
	//pthread_t hiloEscuchadorFM9= crearHilo(&escuchar,FM9);

	esperarHilo(hiloConsola);
	esperarHilo(hiloEscuchadorSAFA);
	//esperarHilo(hiloEscuchadorFM9);

	return 0;
}
