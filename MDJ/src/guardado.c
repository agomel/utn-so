#include "guardado.h"
void verificarExistenciaDeBitmap(){

	char* rutaBitmap = concatenar(PUNTO_MONTAJE_METADATA, "Bitmap.bin");
	int archivoExiste = validarArchivo(rutaBitmap);
	if(archivoExiste != 0){
		//El bitmap no existe
		int archivoCreado = crearArchivoSinBarraN(rutaBitmap);
		if(archivoCreado == 0){
			char* datos = asignarMemoria(CANTIDAD_BLOQUES) + 1;
			for(int i = 0; i<CANTIDAD_BLOQUES; i++){
				datos[i] = '0';
			}

			datos[CANTIDAD_BLOQUES] = EOF;
			guardarDatos(rutaBitmap, 0, CANTIDAD_BLOQUES + 1, datos);

		}
		log_info(logger, "creado bitmap para %d bloques", CANTIDAD_BLOQUES);
	}
	free(rutaBitmap);
}
