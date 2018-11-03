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
	enviarYSerializarCharSinHeader(socketMDJ, OBTENER_DATOS);
	enviarYSerializarStringSinHeader(socketMDJ, path);
	enviarYSerializarIntSinHeader(socketMDJ, -1); //Obtener todo el archivo (no mando el size)
	int tamanioARecibirTotal = deserializarInt(socketMDJ);
	int tamanioRecibido = 0;
	char* escriptorio;
	escriptorio = asignarMemoria(tamanioARecibirTotal);
	int desplazamiento = 0;
	int tamanioARecibir;

	while(tamanioRecibido< tamanioARecibirTotal){
		if(tamanioARecibirTotal - tamanioRecibido > transferSize){
			tamanioARecibir = transferSize;
		}else{
			tamanioARecibir = tamanioARecibirTotal - tamanioRecibido;
		}
		char* parteRecibida = deserializarStringSinElInt(socketMDJ, tamanioARecibir);
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
	int cantidadTotal = (sizeof(datos)+1) ;
	enviarYSerializarCharSinHeader(socketMDJ, GUARDAR_DATOS);
	enviarYSerializarStringSinHeader(socketMDJ, datos);
	return deserializarInt(socketMDJ);
}



