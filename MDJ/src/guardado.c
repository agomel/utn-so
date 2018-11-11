#include "guardado.h"
void verificarExistenciaDeBitmap(){

	int archivoExiste = validarArchivo(rutaBitmap);
	if(archivoExiste != 0){
		//El bitmap no existe
		int archivoCreado = crearArchivoSinBarraN(rutaBitmap);
		if(archivoCreado == 0){
			char* datos = asignarMemoria(CANTIDAD_BLOQUES) + 1;
			for(int i = 0; i<CANTIDAD_BLOQUES; i++){
				datos[i] = '0';
			}

			guardarDatos(rutaBitmap, 0, CANTIDAD_BLOQUES + 1, datos);

		}
		log_info(logger, "creado bitmap para %d bloques", CANTIDAD_BLOQUES);
	}
}

int obtenerBloqueLibreBitmap(){
	char* bitmap = obtenerDatos(rutaBitmap, 0, -1);
	int posicion;
	for(int i = 0; i < strlen(bitmap) + 1; i++){
		if(bitmap[i] == '0'){
			posicion = i;
			log_info(logger, "bloque libre encontrado en posicion %d", posicion);
			break;
		}
	}
	return posicion;
}

void ocuparBloqueEnBitmap(int bloqueAOcupar){
	guardarDatos(rutaBitmap, bloqueAOcupar, sizeof(char), "1");
}
void liberarBloqueEnBitmap(int bloqueAOcupar){
	guardarDatos(rutaBitmap, bloqueAOcupar, sizeof(char), "0");
}

char* crearRutaBloque(int* bloque){
	char* rutaArchivo = asignarMemoria(strlen(PUNTO_MONTAJE_BLOQUES) + 1);
	memcpy(rutaArchivo, PUNTO_MONTAJE_BLOQUES, strlen(PUNTO_MONTAJE_BLOQUES) + 1);

	*bloque = obtenerBloqueLibreBitmap();
	ocuparBloqueEnBitmap(*bloque);
	string_append(&rutaArchivo, intToString(*bloque));
	string_append(&rutaArchivo, ".bin");
	return rutaArchivo;
}
crearArchivoEnBloques(char* datosTotales){
	int tamanioTotal = strlen(datosTotales) + 1;
	int tamanioEscrito = 0;
	while(tamanioEscrito < tamanioTotal){
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
		int creacionDeArchivo = crearArchivoSinBarraN(rutaBloque);
		if(creacionDeArchivo == 0){
			char* textoAEscribir = asignarMemoria(tamanioAEscribir);
			strncpy(textoAEscribir, datosTotales + tamanioEscrito, tamanioAEscribir);
			guardarDatos(rutaBloque, 0, tamanioAEscribir, textoAEscribir);
			tamanioEscrito += tamanioAEscribir;
		}else{
			log_info(logger, "No se pudo crear el archivo");
		}


	}
}
void initGuardado(){
	rutaBitmap = asignarMemoria(strlen(PUNTO_MONTAJE_METADATA) + 1);
	memcpy(rutaBitmap, PUNTO_MONTAJE_METADATA, strlen(PUNTO_MONTAJE_METADATA) + 1);
	string_append(&rutaBitmap, "Bitmap.bin");

}
