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

char* obtenerDatosDeMDJ(int transferSize){
	int offset = 0;
	/*char* datosTotales = asignarMemoria(0);

	int cantidadDeDatos = transferSize;
	while(cantidadDeDatos == transferSize){
		void* buffer = asignarMemoria(sizeof(char) + sizeof(int) + (strlen(operacion->path)+1)
				+ sizeof(int) + sizeof(int));
		int desplazamiento = 0;

		concatenarChar(buffer, &desplazamiento, OBTENER_DATOS);
		concatenarString(buffer, &desplazamiento, operacion->path);
		concatenarInt(buffer, &desplazamiento, offset);
		concatenarInt(buffer, &desplazamiento, transferSize);

		enviarMensaje(socketMDJ, buffer, desplazamiento);
		free(buffer);

		cantidadDeDatos = deserializarInt(socketMDJ);
		char* datosParciales = asignarMemoria(cantidadDeDatos);

		datosTotales = realloc(datosTotales, sizeof(datosTotales) + cantidadDeDatos);
		recibirMensaje(socketMDJ, datosParciales, cantidadDeDatos);

		offset = offset + cantidadDeDatos;
		//TODO ver porque rompe esto
	}*/
	//ESTO es jarcodeado
	char* datosTotales = "estos son los datos";
	return datosTotales;
}

int crearArchivoEnMDJ(char* path, int cantidadDeLineas){
	void* buffer = asignarMemoria(sizeof(char) + sizeof(int) + (strlen(path)+1) + sizeof(int));
	int desplazamiento = 0;
	concatenarChar(buffer, &desplazamiento, CREAR_ARCHIVO);
	concatenarString(buffer, &desplazamiento, path);
	concatenarInt(buffer, &desplazamiento, cantidadDeLineas);
	enviarMensaje(socketMDJ, buffer, desplazamiento);
	free(buffer);
	return deserializarInt(socketMDJ);
}

int guardarDatosEnMDJ(void* datosTotales){
	int cantidadTotal = sizeof(datosTotales);
	enviarYSerializarInt(socketMDJ, cantidadTotal, GUARDAR_DATOS);
	int desplazamiento = 0;
	while(cantidadTotal > 0){
		int cantidadAEnviar;
		if(cantidadTotal > transferSize){
			cantidadAEnviar = transferSize;
		}else{
			cantidadAEnviar = cantidadTotal;
		}
		enviarMensaje(socketMDJ, datosTotales + desplazamiento, cantidadAEnviar);
		desplazamiento = desplazamiento + cantidadAEnviar;
		cantidadTotal = cantidadTotal - cantidadAEnviar;
	}
	return deserializarInt(socketMDJ);
}



