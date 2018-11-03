#include "ServicioFM9.h"


int enviarDatosAFM9(int idDTB, char* path, char* datos, char header){
	int tamanioBuffer = sizeof(int) + sizeof(char) + sizeof(int) + strlen(path)+1 + sizeof(int) + strlen(datos)+1;
	void* buffer = asignarMemoria(tamanioBuffer);
	int desplazamiento = 0;

	concatenarChar(buffer, &desplazamiento, header);
	concatenarString(buffer, &desplazamiento, path);
	concatenarString(buffer, &desplazamiento, datos);

	enviarMensaje(socketFM9, buffer, tamanioBuffer);
	free(buffer);

	return deserializarInt(socketFM9);
}

void pedirDatosAFM9(int idDTB, char* path){
	void* buffer = asignarMemoria(sizeof(int) + sizeof(char) + sizeof(int) + strlen(path)+1);
	int desplazamiento = 0;

	concatenarChar(buffer, &desplazamiento, OBTENER_DATOS);
	concatenarListaInt(buffer, &desplazamiento, path);
	enviarMensaje(socketFM9, buffer, desplazamiento);
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

t_list* recibirListaDeDireccionesDeFM9(){
	return deserializarListaInt(socketFM9);
}
