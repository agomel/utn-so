#include "operacionesFIFA.h"

char* generarPathAbsoluto(char* path){
	if(strlen(path) > 0 && path[0] == '/'){
		memcpy(path, path + 1, strlen(path));
	}
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

	char* datos = asignarMemoria(cantidadDeBytes + 1);
	for(int i = 0; i < cantidadDeBytes; i++){
		datos[i] = '\n';
	}
	datos[cantidadDeBytes] = '\0';
	int estadoCrearArchivoFS = crearArchivoFS(rutaCompleta, datos);

	free(datos);
	free(rutaCompleta);
	return estadoCrearArchivoFS;
}

char* obtenerDatosFIFA(char* rutaArchivo, int offset, int size){
	char* rutaCompleta = generarPathAbsoluto(rutaArchivo);

	char* datos = obtenerDatosFS(rutaCompleta, offset, size);

	free(rutaCompleta);
	return datos;
 }

int guardarDatosFIFA(char* rutaArchivo, int offset, int tamanioMensaje, char* datos){
	char* rutaCompleta = generarPathAbsoluto(rutaArchivo);

	int estadoDeGuardado = guardarDatosFS(rutaCompleta, offset, tamanioMensaje, datos);

	free(rutaCompleta);
	return estadoDeGuardado;
}

int borrarArchivoFIFA(char* rutaArchivo){
	char* rutaCompleta = generarPathAbsoluto(rutaArchivo);

	int estadoDeBorrado = borrarArchivoFS(rutaCompleta);

	free(rutaCompleta);
	return estadoDeBorrado;
}
