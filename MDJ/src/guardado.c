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

void initGuardado(){
	rutaBitmap = asignarMemoria(strlen(PUNTO_MONTAJE_METADATA) + 1);
	memcpy(rutaBitmap, PUNTO_MONTAJE_METADATA, strlen(PUNTO_MONTAJE_METADATA) + 1);
	string_append(&rutaBitmap, "Bitmap.bin");
}
