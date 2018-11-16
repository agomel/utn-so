#include "ServicioSAFA.h"

void notificarASafaExito(char header ,int idDTB, char* path){
	int tamanioBuffer = sizeof(char) + sizeof(int) + sizeof(int) + strlen(path)+1;
	void* buffer = asignarMemoria(tamanioBuffer);
	int desplazamiento = 0;
	concatenarChar(buffer, &desplazamiento, header);
	concatenarInt(buffer, &desplazamiento, idDTB);
	concatenarString(buffer, &desplazamiento, path);
	log_info(logger,"enviando mensaje a safa");
	enviarMensaje(socketSAFA, buffer, desplazamiento);
	free(buffer);
}

void notificarASafaExitoConPesoArchivo(char header ,int idDTB, char* path, int pesoArchivo){
	int tamanioBuffer = sizeof(char) + sizeof(int) + sizeof(int) + strlen(path)+1 + sizeof(int);
	void* buffer = asignarMemoria(tamanioBuffer);
	int desplazamiento = 0;
	concatenarChar(buffer, &desplazamiento, header);
	concatenarInt(buffer, &desplazamiento, idDTB);
	concatenarString(buffer, &desplazamiento, path);
	concatenarInt(buffer, &desplazamiento, pesoArchivo);
	log_info(logger,"enviando mensaje a safa");
	enviarMensaje(socketSAFA, buffer, desplazamiento);
	free(buffer);
}


void enviarError(int idDTB, char* path, int error){
	void* buffer = asignarMemoria(sizeof(char) + sizeof(int)*3 + strlen(path)+1);
	int desplazamiento = 0;
	concatenarChar(buffer, &desplazamiento, ERROR);
	concatenarInt(buffer, &desplazamiento, idDTB);
	concatenarString(buffer, &desplazamiento, path);
	concatenarInt(buffer, &desplazamiento, error);
	enviarMensaje(socketSAFA, buffer, desplazamiento);
	free(buffer);
}


