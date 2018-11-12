#include "bloques.h"


char* crearRutaBloque(int* bloque){
	*bloque = obtenerBloqueLibreBitmap();
	ocuparBloqueEnBitmap(*bloque);

	char* rutaArchivo = concatenar(PUNTO_MONTAJE_BLOQUES, intToString(*bloque));
	concatenarATexto(&rutaArchivo, ".bin");
	return rutaArchivo;
}
int guardarDatosEnBloque(char* rutaBloque, int tamanioAEscribir, int tamanioEscrito, char* datosTotales){
	int creacionDeArchivo = crearArchivo(rutaBloque);
	if(creacionDeArchivo == 0){
		char* textoAEscribir = asignarMemoria(tamanioAEscribir);
		strncpy(textoAEscribir, datosTotales + tamanioEscrito, tamanioAEscribir);
		guardarDatos(rutaBloque, 0, tamanioAEscribir, textoAEscribir);
		return tamanioAEscribir;
	}else{
		log_info(logger, "No se pudo crear el archivo");
		return 0;
	}
}
t_list* crearArchivoEnBloques(char* datosTotales){
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

		int bloqueAEscribir;
		char* rutaBloque = crearRutaBloque(&bloqueAEscribir);
		list_add(bloques, bloqueAEscribir);
		tamanioEscrito += guardarDatosEnBloque(rutaBloque, tamanioAEscribir, tamanioEscrito, datosTotales);

	}
	return bloques;
 }
