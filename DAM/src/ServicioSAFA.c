#include "ServicioSAFA.h"


void notificarASafaExitoDeCarga(int idDTB, char* path, t_list* direcciones){
	void* buffer =
			sizeof(char) + sizeof(int) + sizeof(int) + strlen(path)+1 + sizeof(int) + direcciones->elements_count*sizeof(int);
	int desplazamiento = 0;
	concatenarChar(buffer, &desplazamiento, CARGADO_CON_EXITO_EN_MEMORIA);
	concatenarInt(buffer, &desplazamiento, idDTB);
	concatenarString(buffer, &desplazamiento, path);
	concatenarListaInt(buffer, &desplazamiento, direcciones);
	enviarMensaje(socketSAFA, buffer, desplazamiento);
	free(buffer);
}

void notificarASafaExitoDeGuardado(int idDTB, char* path){
	void* buffer = asignarMemoria(sizeof(char) + sizeof(int) + strlen(path)+1);
	int desplazamiento = 0;
	concatenarChar(buffer, &desplazamiento, GUARDADO_CON_EXITO_EN_MDJ);
	concatenarInt(buffer, &desplazamiento, idDTB);
	concatenarString(buffer, &desplazamiento, path);
	enviarMensaje(socketSAFA, buffer, desplazamiento);
	free(buffer);
}

void enviarError(int idDTB, char* path, int error){
	void* buffer = asignarMemoria(sizeof(char) + sizeof(int) + strlen(path)+1);
	int desplazamiento = 0;
	concatenarChar(buffer, &desplazamiento, ERROR);
	concatenarInt(buffer, &desplazamiento, idDTB);
	concatenarString(buffer, &desplazamiento, path);
	concatenarInt(buffer, &desplazamiento, error);
	enviarMensaje(socketSAFA, buffer, desplazamiento);
	free(buffer);
}


