#include "utilidades.h"
void* asignarMemoria(int cantidad){
	void* buffer = malloc(cantidad);
	if(buffer == NULL){
		printf("No hay espacio\n");
		exit(-99);
	}
	return buffer;
}

char** deserializar(int emisor){
	printf("Entro a deserialezar");
	int espacioALeer;
	int parametro=0;
	char** parametros;
	char* buffer;

	buffer=asignarMemoria(3);
	recibirMensaje(emisor, &buffer, 2);
	espacioALeer = atoi(buffer);
	printf("espacio a leer %d",espacioALeer);
	while(espacioALeer != 99){
		parametros[parametro]=asignarMemoria(espacioALeer +1);
		recibirMensaje(emisor, &parametros[parametro], espacioALeer);
		printf("parametro es %s",parametros[parametro]);

		free(buffer);
		buffer=asignarMemoria(3);
		recibirMensaje(emisor, &buffer, 2);
		espacioALeer = atoi(buffer);
		printf("espacio a leer %d", espacioALeer);
	}

	return parametros;
}
