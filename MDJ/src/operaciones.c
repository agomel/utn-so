#include "operaciones.h"

int validarArchivo(int emisor){
	char* rutaArchivo = deserializarString(emisor);
	log_info(logger, "Validando archivo en ruta: %s", rutaArchivo);
	FILE* archivo = fopen(rutaArchivo, "r");
	if(archivo == NULL){
		return PATH_INEXISTENTE;
	}
	//0 es archivo valido
	return 0;
}

int crearArchivo(int emisor){
	char* rutaArchivo = deserializarString(emisor);
	int cantidadDeBytes = deserializarInt(emisor);
	log_info(logger, "Creando archivo en ruta: %s", rutaArchivo);
	//TODO verificar si hay espacio antes de crearlo y esa movida
	//TODO se tiene que crear con esa cantidad de bytes.. debe ser para el bitmap
	FILE* archivo = fopen(rutaArchivo, "w");
	if(archivo == NULL){
		return PATH_INEXISTENTE;// nose que error podria pasar aca
	}
	return 0;
}

void guardarDatos(int emisor){
	char* rutaArchivo = deserializarString(emisor);
	char* offset = deserializarString(emisor);
	int tamanioALeer = deserializarInt(emisor);
	char* datos = deserializarString(emisor);
	log_info("Guardando datos: %s en archivo: %s", datos, rutaArchivo);
	//TODO guardar datos posta y hacer free de los strings
}

char* obtenerDatos(int emisor){
	char* rutaArchivo = deserializarString(emisor);
	int offset = deserializarInt(emisor);
	int tamanioALeer = deserializarInt(emisor);
	log_info("Obteniendo %d bytes de archivo: %s", tamanioALeer, rutaArchivo);
	return "asginar hola\nwait hola\nsignal hola\n";
	//TODO leer archivos posta y hacer free de los strings
 }

int eliminarArchivo(int emisor){
	char* rutaArchivo = deserializarString(emisor);
	if(remove(rutaArchivo) == 0){
		return 0;
	}else{
		//No se pudo eliminar el archivo
		return PATH_INEXISTENTE;
	}
}

