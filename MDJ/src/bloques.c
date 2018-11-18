#include "bloques.h"

int obtenerCantidadBloques(int tamanio){
	int enteros = tamanio / TAMANIO_BLOQUES;
	int resto = tamanio % TAMANIO_BLOQUES;
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
	char* rutaBloque = getPathDeBloque(bloque);
	crearArchivo(rutaBloque);
	free(rutaBloque);
	return bloque;
}

int eliminarBloque(int bloque){
	return liberarBloqueEnBitmap(bloque);
}

char* obtenerDatosDeBloque(int bloque, int offset, int size){
	char* rutaBloque = getPathDeBloque(bloque);
	char* datosDelBloque = obtenerDatos(rutaBloque, offset, size);
	free(rutaBloque);
	return datosDelBloque;
}

int guardarDatosEnBloque(int bloque, int offset, int size, char* datos){
	char* rutaBloque = getPathDeBloque(bloque);
	return guardarDatos(rutaBloque, offset, size, datos);
}

t_list* crearArchivoEnBloques(char* datosTotales){
	t_list* bloques = list_create();
	int tamanioTotal = strlen(datosTotales);
	int tamanioEscrito = 0;
	int tamanioAEscribir;

	while(tamanioTotal > 0){
		if(tamanioTotal < TAMANIO_BLOQUES){
			//Datos a guardar no superan el tamanio de bloque
			tamanioAEscribir = tamanioTotal;
		}else{
			//Datos a guardar superan el de un bloque
			tamanioAEscribir = TAMANIO_BLOQUES;
		}

		int bloqueAEscribir = obtenerBloque();
		list_add(bloques, bloqueAEscribir);

		guardarDatosEnBloque(bloqueAEscribir, 0, tamanioAEscribir, (datosTotales + tamanioEscrito));

		tamanioEscrito += tamanioAEscribir;
		tamanioTotal -= tamanioAEscribir;
	}

	return bloques;
 }

