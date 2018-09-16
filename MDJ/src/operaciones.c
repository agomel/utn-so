#include "operaciones.h"

u_int32_t validarArchivo(int emisor){
	char* rutaArchivo = deserializarString(emisor);
	return true;
}
void crearArchivo(int emisor){}

char* obtenerDatos(u_int32_t emisor){
	char* rutaArchivo = deserializarString(emisor);
	char* desplazamiento = deserializarString(emisor);
	int tamanioALeer = deserializarInt(emisor);

	return "leyendo de archivo";
 }
