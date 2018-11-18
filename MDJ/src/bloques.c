#include "bloques.h"

int obtenerCantidadBloques(int tamanio){
	int enteros = tamanio / CANTIDAD_BLOQUES;
	int resto = tamanio % CANTIDAD_BLOQUES;
	if(resto > 0){
		enteros ++;
	}
	return enteros;
}

char* getPathDeBloque(int bloque){
	char* rutaArchivo = concatenar(PUNTO_MONTAJE_BLOQUES, intToString(bloque));
	concatenarATexto(&rutaArchivo, ".bin");
	return rutaArchivo;
}

int obtenerBloque(){
	int bloque = obtenerBloqueLibreBitmap();
	ocuparBloqueEnBitmap(bloque);
	return bloque;
}

int eliminarBloque(int bloque){
	int error = 0;
	char* rutaArchivoDeBloque = getPathDeBloque(bloque);

	error = eliminarArchivo(rutaArchivoDeBloque);
	if(error == 0){
		error = liberarBloqueEnBitmap(bloque);
	}

	free(rutaArchivoDeBloque);
	return error;
}

int guardarDatosEnBloque(char* rutaBloque, int tamanioAEscribir, int tamanioEscrito, char* datosTotales, int* error){
	int creacionDeArchivo = crearArchivo(rutaBloque);
	if(creacionDeArchivo == 0){
		char* textoAEscribir = asignarMemoria(tamanioAEscribir);
		strncpy(textoAEscribir, datosTotales + tamanioEscrito, tamanioAEscribir);
		*error = guardarDatos(rutaBloque, 0, tamanioAEscribir, textoAEscribir);
		return tamanioAEscribir;
	}else{
		*error = PATH_INEXISTENTE;
		return 0;
	}
}
t_list* crearArchivoEnBloques(char* datosTotales, int* error){
	t_list* bloques = list_create();
	int tamanioTotal = strlen(datosTotales) + 1;
	int tamanioEscrito = 0;
	while(tamanioTotal > tamanioEscrito){
		int tamanioAEscribir;
		if(tamanioTotal - tamanioEscrito < TAMANIO_BLOQUES){
			//Datos a guardar no alcanzan tamanio de bloque
			tamanioAEscribir = tamanioTotal - tamanioEscrito;
		}else{
			//Datos a guardar superan el de un bloque
			tamanioAEscribir = TAMANIO_BLOQUES;
		}

		int bloqueAEscribir = obtenerBloque();
		char* rutaBloque = getPathDeBloque(bloqueAEscribir);
		list_add(bloques, bloqueAEscribir);
		tamanioEscrito += guardarDatosEnBloque(rutaBloque, tamanioAEscribir, tamanioEscrito, datosTotales, error);

	}
	return bloques;
 }

char* obtenerDatosDeBloque(int bloque, int offset, int size){
	char* rutaBloque = getPathDeBloque(bloque);
	return obtenerDatos(rutaBloque, offset, size);
}
