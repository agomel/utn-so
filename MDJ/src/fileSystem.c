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

	//liberar bloques del bitmap

	//borrar archivo con la metadata

	//contemplar archivo ineficiente

	return 0;

}



