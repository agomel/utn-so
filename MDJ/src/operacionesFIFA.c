#include "operacionesFIFA.h"

char* generarPathAbsoluto(char* path){
	return concatenar(PUNTO_MONTAJE_ARCHIVOS, path);
}

int validarArchivoFIFA(char* rutaArchivo){
	char* rutaCompleta = generarPathAbsoluto(rutaArchivo);
	int estadoDeOperacion = validarArchivo(rutaCompleta);
	free(rutaCompleta);
	return estadoDeOperacion;
}

int crearArchivoFIFA(char* rutaArchivo, int cantidadDeBytes){
	//TODO verificar si hay espacio antes de crearlo y esa movida de los bloques en el bitmap

	char* rutaCompleta = generarPathAbsoluto(rutaArchivo);

	int creacionDeArchivo = crearArchivo(rutaCompleta);

	if(creacionDeArchivo == 0){
		char* datos = asignarMemoria(cantidadDeBytes) + 1;
		for(int i = 0; i < cantidadDeBytes; i++){
			datos[i] = '\n';
		}
		guardarDatos(rutaArchivo, 0, cantidadDeBytes, datos);
	}

	free(rutaCompleta);
	return creacionDeArchivo;




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




char* obtenerDatosFIFA(char* rutaArchivo, int offset, int size){
	char* rutaCompleta = generarPathAbsoluto(rutaArchivo);

	char* datos = obtenerDatos(rutaCompleta, offset, size);

	free(rutaCompleta);
	return datos;
 }

int guardarDatosFIFA(char* rutaArchivo, int offset, int tamanioMensaje, char* datos){
	char* rutaCompleta = generarPathAbsoluto(rutaArchivo);

	int estadoDeGuardado = guardarDatos(rutaArchivo, offset, tamanioMensaje, datos);

	free(rutaCompleta);
	return estadoDeGuardado;
}

int borrarArchivoFIFA(char* rutaArchivo){
	char* rutaCompleta = generarPathAbsoluto(rutaArchivo);

	//liberar los bloques del bitmap
	//eliminar el archivo con la metadata
	//contemplar el caso de que no exista el archivo, de ser asi, devolver error de archivo inexistente

	int estadoDeBorrado = eliminarArchivo(rutaCompleta);

	free(rutaCompleta);
	return estadoDeBorrado;
}
