#include "operaciones.h"

int validarArchivo(char* rutaArchivo){
	log_info(logger, "validando archivo en ruta %s", rutaArchivo);
	FILE* archivo = fopen(rutaArchivo, "r");
	if(archivo == NULL || archivo < 0){
		return PATH_INEXISTENTE;
	}
	fclose(archivo);
	//0 es archivo valido
	return 0;
}

int crearArchivoSinBarraN(char* rutaArchivo){
	//TODO verificar si hay espacio antes de crearlo y esa movida de los bloques en el bitmap

	log_info(logger, "creando archivo en ruta %s", rutaArchivo);
	FILE* archivo = fopen(rutaArchivo, "w");
	if(archivo == NULL){
		log_info(logger, "Error gato!");
		return PATH_INEXISTENTE;
	}

	fclose(archivo);
	return 0;
}

int crearArchivo(char* rutaArchivo, int cantidadDeBytes){
	//TODO verificar si hay espacio antes de crearlo y esa movida de los bloques en el bitmap

	FILE* archivo;
	int creacionDeArchivo = crearArchivoSinBarraN(rutaArchivo);
	if(creacionDeArchivo == 0){
		char* datos = asignarMemoria(cantidadDeBytes) + 1;
		for(int i = 0; i < cantidadDeBytes; i++){
			datos[i] = '\n';
		}
		guardarDatos(rutaArchivo, 0, cantidadDeBytes + 1, datos);

	}
	return creacionDeArchivo;
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

char* obtenerDatos(char* rutaArchivo, int offset, int size){
	int myFile = open(rutaArchivo, O_RDONLY);
	if(myFile < 0){
		log_info(logger, "Error al abrir el archivo %s", rutaArchivo);
	}

	lseek(myFile, offset, SEEK_SET);

	if(size < 0){
		struct stat myStat;
		if(fstat(myFile, &myStat)){
			log_info(logger, "Error al ver el estado del archivo %s", rutaArchivo);
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

