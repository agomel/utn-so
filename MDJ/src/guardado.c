#include "guardado.h"
void verificarExistenciaDeBitmap(){

	char* rutaBitmap = asignarMemoria(strlen(PUNTO_MONTAJE_METADATA) + 1);
	memcpy(rutaBitmap, PUNTO_MONTAJE_METADATA, strlen(PUNTO_MONTAJE_METADATA) + 1);
	string_append(&rutaBitmap, "Bitmap.bin");

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
	free(rutaBitmap);
}
