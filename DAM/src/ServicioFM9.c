#include "ServicioFM9.h"


int enviarDatosAFM9(char* path, char* datos){
	int tamanioBuffer = sizeof(char) + sizeof(int) + strlen(path)+1 + sizeof(int) + strlen(datos)+1;
	void* buffer = asignarMemoria(tamanioBuffer);
	int desplazamiento = 0;

	concatenarChar(buffer, &desplazamiento, GUARDAR_DATOS);
	concatenarString(buffer, &desplazamiento, path);
	concatenarString(buffer, &desplazamiento, datos);

	enviarMensaje(socketFM9, buffer, tamanioBuffer);
	free(buffer);

	return deserializarInt(socketFM9);
}

int pedirDatosAFM9(char* path){
	void* buffer = asignarMemoria(sizeof(char) + sizeof(int) + strlen(path)+1);
	int desplazamiento = 0;

	concatenarChar(buffer, &desplazamiento, OBTENER_DATOS);
	concatenarListaInt(buffer, &desplazamiento, path);
	enviarMensaje(socketFM9, buffer, desplazamiento);
	free(buffer);

	return deserializarInt(socketFM9);
}

char* recibirFlushFM9(){
	int cantidadTotal = deserializarInt(socketFM9);
	char* memoriaTotal = asignarMemoria(cantidadTotal);
	int desplazamiento = 0;
	while(cantidadTotal > 0){
		int cantidadARecibir;
		if(cantidadTotal > transferSize){
			cantidadARecibir = transferSize;
		}else{
			cantidadARecibir = cantidadTotal;
		}
		void* buffer = asignarMemoria(cantidadARecibir);
		recibirMensaje(socketFM9, buffer, cantidadARecibir);
		concatenarVoid(memoriaTotal, &desplazamiento, buffer, cantidadARecibir);
		free(buffer);
		cantidadTotal = cantidadTotal - cantidadARecibir;
	}
	return memoriaTotal;
}

t_list* recibirListaDeDireccionesDeFM9(){
	return deserializarListaInt(socketFM9);
}

