#include "operaciones.h"

int validarArchivo(char* rutaArchivo){
	FILE* archivo = fopen(rutaArchivo, "r");
	if(archivo < 0){
		return PATH_INEXISTENTE;
	}
	fclose(archivo);
	//0 es archivo valido
	return 0;
}

int crearArchivo(char* rutaArchivo, int cantidadDeBytes){
	//TODO verificar si hay espacio antes de crearlo y esa movida de los bloques en el bitmap

	FILE* archivo = fopen(rutaArchivo, "w");
	if(archivo == NULL){
		log_info(logger, "Error gato!");
		return PATH_INEXISTENTE;
	}
	for(int i = 0; i < cantidadDeBytes; i++){
		fputs("\n", archivo);
	}

	fclose(archivo);
	return 0;
}

int guardarDatos(char* rutaArchivo, int offset, int tamanioMensaje, char* datos){
	int myFile = open(rutaArchivo, O_WRONLY);
	if(myFile < 0){
		log_info(logger, "Error gato!");
	}

	lseek(myFile, offset, SEEK_SET);

	write(myFile, datos, tamanioMensaje);

	close(myFile);

	return 0;
}

char* obtenerDatos(char* rutaArchivo, int offset, int tamanioALeer){
	int myFile = open(rutaArchivo, O_RDONLY);
	if(myFile < 0){
		log_info(logger, "Error Al abrir el archivo!");
	}

	lseek(myFile, offset, SEEK_SET);

	int size = tamanioALeer;

	if(size < 0){
		struct stat myStat;
		if(fstat(myFile, &myStat)){
			log_info(logger, "Error al ver el estado del archivo!");
		}
		size = myStat.st_size;
	}

	char* buffer = asignarMemoria(size);
	read(myFile, buffer, size);

	close(myFile);

	return buffer;
 }

int eliminarArchivo(char* rutaArchivo){
	if(remove(rutaArchivo) == 0){
		return 0;
	}else{
		//No se pudo eliminar el archivo
		return PATH_INEXISTENTE;
	}
}

