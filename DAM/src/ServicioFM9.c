#include "ServicioFM9.h"


int enviarDatosAFM9(int idDTB, char* path, char* datos, char header){
	int tamanioBuffer = sizeof(int) + sizeof(char) + sizeof(int) + strlen(path)+1 + sizeof(int) + strlen(datos)+1;
	void* buffer = asignarMemoria(tamanioBuffer);
	int desplazamiento = 0;

	concatenarChar(buffer, &desplazamiento, header);
	concatenarInt(buffer, &desplazamiento, idDTB);
	concatenarString(buffer, &desplazamiento, path);
	concatenarString(buffer, &desplazamiento, datos);

	enviarMensaje(socketFM9, buffer, tamanioBuffer);
	free(buffer);

	return deserializarInt(socketFM9);
}

void pedirDatosAFM9(int idDTB, char* path){
	int tamanioBuffer = sizeof(int) + sizeof(char) + sizeof(int) + strlen(path)+1;
	void* buffer = asignarMemoria(tamanioBuffer);
	int desplazamiento = 0;

	concatenarChar(buffer, &desplazamiento, OBTENER_DATOS);
	concatenarInt(buffer, &desplazamiento, idDTB);
	concatenarString(buffer, &desplazamiento, path);
	enviarMensaje(socketFM9, buffer, desplazamiento);
	free(buffer);
}

char* recibirFlushFM9(int tamanioArchivo){
	char* archivo = string_new();
	int cantidadARecibir = 0;
	while(tamanioArchivo > 0){
		if(tamanioArchivo > transferSize)
			cantidadARecibir = transferSize;
		else
			cantidadARecibir = tamanioArchivo;

		char* buffer = deserializarStringSinElIntDAM(socketFM9, cantidadARecibir);
		string_append(&archivo, buffer);

		free(buffer);
		tamanioArchivo -= cantidadARecibir;
	}
	return archivo;
}

