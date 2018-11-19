#include "operacionesArchivos.h"

int validarArchivo(char* rutaArchivo){
	FILE* archivo = fopen(rutaArchivo, "r");
	if(archivo == NULL || archivo < 0){
		return PATH_INEXISTENTE;
	}
	fclose(archivo);
	return 0;
}

int crearArchivo(char* rutaArchivo){
	FILE* archivo = fopen(rutaArchivo, "w");
	if(archivo == NULL){
		return ESPACIO_INSUFICIENTE_EN_MDJ;
	}
	fclose(archivo);
	return 0;
}

int guardarDatos(char* rutaArchivo, int offset, int tamanioMensaje, char* datos){
	int myFile = open(rutaArchivo, O_WRONLY);
	if(myFile < 0){
		return ESPACIO_INSUFICIENTE_EN_MDJ;
	}

	lseek(myFile, offset, SEEK_SET);

	write(myFile, datos, tamanioMensaje);

	close(myFile);

	return 0;
}

char* obtenerDatos(char* rutaArchivo, int offset, int size){
	int myFile = open(rutaArchivo, O_RDONLY);
	if(myFile < 0){
		log_info(logger, "Error al abrir el archivo %s", rutaArchivo);
		return PATH_INEXISTENTE;
	}

	lseek(myFile, offset, SEEK_SET);

	if(size < 0){
		struct stat myStat;
		if(fstat(myFile, &myStat)){
			log_info(logger, "Error al ver el estado del archivo %s", rutaArchivo);
			return PATH_INEXISTENTE;
		}
		size = myStat.st_size;
	}

	char* buffer = asignarMemoria(size + 1);

	read(myFile, buffer, size);

	close(myFile);
	buffer[size] = '\0';
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

