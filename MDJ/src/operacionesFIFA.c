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
	char* rutaCompleta = generarPathAbsoluto(rutaArchivo);

	char* datos = asignarMemoria(cantidadDeBytes) + 1;
	for(int i = 0; i < cantidadDeBytes; i++){
		datos[i] = '\n';
	}
	int estadoCrearArchivoFS = crearArchivoFS(rutaCompleta, datos);

	free(rutaCompleta);
	return estadoCrearArchivoFS;
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
