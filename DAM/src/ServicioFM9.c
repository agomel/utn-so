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

	int respuestaFM9 = deserializarInt(socketFM9);
	log_debug(logger, "Recibi %d al guardar los datos en FM9", respuestaFM9);
	return respuestaFM9;
}

void pedirDatosAFM9(char* path){
	int tamanioBuffer = sizeof(char) + sizeof(int) + strlen(path)+1;
	void* buffer = asignarMemoria(tamanioBuffer);
	int desplazamiento = 0;

	concatenarChar(buffer, &desplazamiento, OBTENER_DATOS);
	concatenarString(buffer, &desplazamiento, path);
	enviarMensaje(socketFM9, buffer, tamanioBuffer);
	free(buffer);
}

char* recibirFlushFM9(int cantidadDeLineas){
	char* memoriaTotal = asignarMemoria(cantidadDeLineas);
	int desplazamiento = 0;
	while(cantidadDeLineas > 0){
		int cantidadARecibir;
		if(cantidadDeLineas > transferSize){
			cantidadARecibir = transferSize;
		}else{
			cantidadARecibir = cantidadDeLineas;
		}
		void* buffer = asignarMemoria(cantidadARecibir);
		recibirMensaje(socketFM9, buffer, cantidadARecibir);
		concatenarVoid(memoriaTotal, &desplazamiento, buffer, cantidadARecibir);
		free(buffer);
		cantidadDeLineas = cantidadDeLineas - cantidadARecibir;
	}
	return memoriaTotal;
}
