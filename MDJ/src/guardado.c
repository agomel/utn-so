#include "guardado.h"

void crearArchivoFifa(char* path, char* datosTotales){

	char* rutaArchivo = concatenar(PUNTO_MONTAJE_ARCHIVOS, path);
	t_list* bloques = crearArchivoEnBloques(datosTotales);
	char* texto = concatenar("TAMANIO=", intToString(strlen(datosTotales) + 1));
	concatenarATexto(&texto, "\nBLOQUES=[");

	for(int i = 0; i<bloques->elements_count; i++){
		concatenarATexto(&texto, intToString(list_get(bloques, i)));

		if(i == bloques->elements_count -1){
			concatenarATexto(&texto, "]\n");
		}else{
			concatenarATexto(&texto, ",");
		}

	}

	int creacionDeArchivo = crearArchivo(rutaArchivo);
	if(creacionDeArchivo == 0){
		guardarDatos(rutaArchivo, 0, strlen(texto), texto);
	}else{
		log_info(logger, "No se pudo crear el archivo");
	}
	list_destroy(bloques);
	free(texto);
	free(rutaArchivo);


}
