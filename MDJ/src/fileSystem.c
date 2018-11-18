#include "fileSystem.h"

int crearArchivoFS(char* rutaArchivo, char* datosTotales){
	int error = 0;
	t_list* bloques = crearArchivoEnBloques(datosTotales, &error);
	char* texto = concatenar("TAMANIO=", intToString(strlen(datosTotales) + 1));
	concatenarATexto(&texto, "\nBLOQUES=[");

	for(int i = 0; i< bloques->elements_count; i++){
		concatenarATexto(&texto, intToString(list_get(bloques, i)));

		if(i == bloques->elements_count -1){
			concatenarATexto(&texto, "]\n");
		}else{
			concatenarATexto(&texto, ",");
		}

	}

	if(error == 0){
		error = crearArchivo(rutaArchivo);
		if(error == 0){
			error = guardarDatos(rutaArchivo, 0, strlen(texto), texto);
		}else{
			error = ESPACIO_INSUFICIENTE_EN_MDJ;
		}
	}

	list_destroy(bloques);
	free(texto);
	return error;
}

int borrarArchivoFS(char* rutaArchivo){
	int error = 0;
	Metadata* metaData = obtenerMetadata(rutaArchivo);
	int cantidadDeBloques = obtenerCantidadBloques(metaData->tamanio);
	for(int i = 0; i < cantidadDeBloques; i++){
		int bloque = atoi(metaData->bloques[i]);
		eliminarBloque(bloque);
	}
	error = eliminarArchivo(rutaArchivo);
	return error;
}

char* obtenerDatosFS(char* rutaArchivo, int offset, int size){
	Metadata* metaData = obtenerMetadata(rutaArchivo);
	int posicionDelBloque = offset / CANTIDAD_BLOQUES;
	int offsetEnBloque = offset % CANTIDAD_BLOQUES;
	int sizeALeerEnBloque;

	char* datosTotales;

	while(size > 0){
		if(size > (CANTIDAD_BLOQUES - offsetEnBloque)){
			sizeALeerEnBloque = CANTIDAD_BLOQUES - offsetEnBloque;
		}else{
			sizeALeerEnBloque = size;
		}

		int bloque = atoi(metaData->bloques[posicionDelBloque]);
		char * datos = obtenerDatosDeBloque(bloque, offsetEnBloque, sizeALeerEnBloque);
		concatenarATexto(datosTotales, datos);

		free(datos);
		posicionDelBloque++;
		offsetEnBloque = 0;
		size = size - sizeALeerEnBloque;
	}

	return datosTotales;
}

