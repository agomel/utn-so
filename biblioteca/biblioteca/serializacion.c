#include "serializacion.h"

void deserializar(void** resultados, int emisor){
	int espacioALeer;

	int resultado=0;
	void* buffer;

	buffer=asignarMemoria(2);
	recibirMensaje(emisor, &buffer, 2);
	espacioALeer = atoi(buffer);
	while(espacioALeer != 99){
		resultados[resultado]=asignarMemoria(espacioALeer);
		recibirMensaje(emisor, &resultados[resultado], espacioALeer);

		free(buffer);
		buffer=asignarMemoria(2);
		recibirMensaje(emisor, &buffer, 2);
		espacioALeer = atoi(buffer);
		resultado++;
	}
}
char* serializarMensaje(int operacion,char*mensaje){
	char* serializado = asignarMemoria(strlen(mensaje)+6);
	int tamanioMensaje= strlen(mensaje);
	if(operacion<10 && tamanioMensaje<10){
		sprintf(serializado, "0%d0%d",operacion,tamanioMensaje);
	}else if(operacion>10 && tamanioMensaje<10){
		sprintf(serializado, "%d0%d",operacion,tamanioMensaje);
	}else if(operacion<10 && tamanioMensaje>10){
		sprintf(serializado, "0%d%d",operacion,tamanioMensaje);
	}else{
		sprintf(serializado, "%d%d",operacion,tamanioMensaje);
	}

	strcat(serializado,mensaje);

	strcat(serializado,"99");
	return serializado;
}
void deserializarIdentificarse(int emisor, t_dictionary* conexiones){
	char* identificador[1];
	printf("Esto es el identificador \n");
	deserializar((void**)identificador, emisor);
	printf("Me mandaron %s \n", identificador[0]);
	dictionary_put(conexiones, identificador[0], emisor);
	printf("Agregado %s a las conexiones \n", identificador[0]);
}
