#include "operaciones.h"

u_int32_t validarArchivo(int emisor){
	char* rutaArchivo = deserializarString(emisor);
	return true;
}
void crearArchivo(int emisor){}

void guardarDatos(u_int32_t emisor){
	char* rutaArchivo = deserializarString(emisor);
	char* desplazamiento = deserializarString(emisor);
	int tamanioALeer = deserializarInt(emisor);
	//TODO guardar datos.
}
char* obtenerDatos(u_int32_t emisor){
	char* rutaArchivo = deserializarString(emisor);
	char* desplazamiento = deserializarString(emisor);
	int tamanioALeer = deserializarInt(emisor);

	return "leyendo de archivo";
 }
