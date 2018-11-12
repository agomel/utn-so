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

	int estadoDeBorrado = eliminarArchivo(rutaCompleta);

	free(rutaCompleta);
	return estadoDeBorrado;
}
