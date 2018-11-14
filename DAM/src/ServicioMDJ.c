#include "ServicioMDJ.h"

int validarArchivoMDJ(char* path){

	int tamanioMensaje = sizeof(char) + sizeof(int) + strlen(path)+1;
	void* buffer = asignarMemoria(tamanioMensaje);
	int desplazamiento = 0;
	concatenarChar(buffer, &desplazamiento, VALIDAR_ARCHIVO);
	concatenarString(buffer, &desplazamiento, path);
	log_info(logger, "Enviando a MDJ que valide escriptorio %s", path);
	enviarMensaje(socketMDJ, buffer, tamanioMensaje);
	free(buffer);
	log_debug(logger, "Esperando repuesta validacion de MDJ");
	int respuesta = deserializarInt(socketMDJ);
	log_debug(logger, "Recibida respuesta %d de MDJ", respuesta);

	return respuesta;
}

char* obtenerDatosDeMDJ(char* path){
	int tamanioMensaje = sizeof(char) + strlen(path)+1 + sizeof(int)*3;
	void* buffer = asignarMemoria(tamanioMensaje);
	int desplazamiento = 0;

	concatenarChar(buffer, &desplazamiento, OBTENER_DATOS);
	concatenarString(buffer, &desplazamiento, path);
	concatenarInt(buffer, &desplazamiento, 0); //Offset
	concatenarInt(buffer, &desplazamiento, -1);//Size

	enviarMensaje(socketMDJ, buffer, tamanioMensaje);

	int tamanioARecibirTotal = deserializarInt(socketMDJ);
	printf("tamanio a recibir llego como %d\n", tamanioARecibirTotal);
	int tamanioRecibido = 0;
	char* escriptorio;
	escriptorio = asignarMemoria(tamanioARecibirTotal);
	desplazamiento = 0;
	int tamanioARecibir;

	while(tamanioRecibido< tamanioARecibirTotal){
		if(tamanioARecibirTotal - tamanioRecibido > transferSize){
			tamanioARecibir = transferSize;
		}else{
			tamanioARecibir = tamanioARecibirTotal - tamanioRecibido;
		}
		char* parteRecibida = deserializarStringSinElIntDAM(socketMDJ, tamanioARecibir);
		memcpy(escriptorio + desplazamiento, parteRecibida, tamanioARecibir);
		free(parteRecibida);
		desplazamiento = desplazamiento + tamanioARecibir;
		tamanioRecibido += tamanioARecibir;
	}

	return escriptorio;
}

int crearArchivoEnMDJ(int destino, char* path, int cantidadDeLineas){
	enviarySerializarPathyTamanioArchivo(destino, path, cantidadDeLineas);
	return deserializarInt(socketMDJ);
}
int borrarArchivoEnMDJ(char* path){
	void* buffer = asignarMemoria(sizeof(char) + sizeof(int));
	int desplazamiento = 0;
	concatenarChar(buffer, &desplazamiento, BORRAR_ARCHIVO);
	concatenarString(buffer, &desplazamiento, path);
	enviarMensaje(socketMDJ, buffer, desplazamiento);
	free(buffer);
	return deserializarInt(socketMDJ);
}
int guardarDatosEnMDJ(char* datos, char* path){
	void* buffer = asignarMemoria(sizeof(char) + sizeof(int)*3 + strlen(path) + 1 + strlen(datos) + 1);
	int desplazamiento = 0;
	concatenarChar(buffer, &desplazamiento, GUARDAR_DATOS);
	concatenarString(buffer, &desplazamiento, path);
	concatenarInt(buffer, &desplazamiento, 0);
	concatenarString(buffer, &desplazamiento, datos);

	enviarMensaje(socketMDJ, buffer, desplazamiento);
	free(buffer);

	return deserializarInt(socketMDJ);
}



