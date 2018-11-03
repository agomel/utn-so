#include "ServicioMDJ.h"

int validarArchivoMDJ(char* path){
	int tamanioMensaje = sizeof(char) + sizeof(int) + strlen(path)+1;
	void* buffer = asignarMemoria(tamanioMensaje);
	int desplazamiento = 0;
	concatenarChar(buffer, &desplazamiento, VALIDAR_ARCHIVO);
	concatenarString(buffer, &desplazamiento, path);
	log_info(logger, "enviando a emisor %d char: %c y escriptorio %s", socketMDJ, VALIDAR_ARCHIVO, path);
	enviarMensaje(socketMDJ, buffer, tamanioMensaje);
	free(buffer);
	int respuesta =  deserializarInt(socketMDJ);
	return respuesta;
}

char* obtenerDatosDeMDJ(char* path){
	int offset = 0;
	char* datosTotales = asignarMemoria(offset);
	int cantidadDeDatos = transferSize;
	while(cantidadDeDatos == transferSize){
		void* buffer = asignarMemoria(sizeof(char) + sizeof(int) + (strlen(path)+1)
				+ sizeof(int) + sizeof(int));
		int desplazamiento = 0;

		concatenarChar(buffer, &desplazamiento, OBTENER_DATOS);
		concatenarString(buffer, &desplazamiento, path);
		concatenarInt(buffer, &desplazamiento, offset);
		concatenarInt(buffer, &desplazamiento, transferSize);

		enviarMensaje(socketMDJ, buffer, desplazamiento);
		free(buffer);

		cantidadDeDatos = deserializarInt(socketMDJ);
		datosTotales = realloc(datosTotales, offset + cantidadDeDatos);
		recibirMensaje(socketMDJ, datosTotales+offset, cantidadDeDatos);
		offset = offset + cantidadDeDatos;
	}
	return datosTotales;
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
int guardarDatosEnMDJ(char* datos, char* path, int cantidadDeLineas){
	int cantidadTotal = (sizeof(datos)+1) ;
	enviarYSerializarInt(socketMDJ, cantidadTotal, GUARDAR_DATOS);
	int desplazamiento = 0;
	while(cantidadTotal > 0){
		int cantidadAEnviar;
		if(cantidadTotal > transferSize){
			cantidadAEnviar = transferSize;
		}else{
			cantidadAEnviar = cantidadTotal;
		}
		enviarMensaje(socketMDJ, datos + desplazamiento, cantidadAEnviar);
		desplazamiento = desplazamiento + cantidadAEnviar;
		cantidadTotal = cantidadTotal - cantidadAEnviar;
	}

	enviarySerializarPathyCantidadDeLineas(socketMDJ, path, cantidadDeLineas);
	return deserializarInt(socketMDJ);
}



