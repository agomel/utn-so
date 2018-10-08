#include "operaciones.h"

int validarArchivo(int emisor){
	char* rutaArchivo = deserializarString(emisor);
	return true;
	//TODO validar el archivo posta
}
void crearArchivo(int emisor){
	//TODO crear el archivo posta
}

void guardarDatos(int emisor){
	char* rutaArchivo = deserializarString(emisor);
	char* offset = deserializarString(emisor);
	int tamanioALeer = deserializarInt(emisor);
	char* datos = deserializarString(emisor);
	printf("Guardados datos recibidos de %d \n" , emisor);
	//TODO guardar datos posta y hacer free de los strings
}
char* obtenerDatos(int emisor){
	char* rutaArchivo = deserializarString(emisor);
	int offset = deserializarInt(emisor);
	int tamanioALeer = deserializarInt(emisor);
	return "leyendo de archivo";
	//TODO leer archivos posta y hacer free de los strings
 }
