#include "utilidades.h"

void* asignarMemoria(int cantidad){
	void* buffer = malloc(cantidad);
	if(buffer == NULL){
		printf("No hay espacio\n");
		exit(-99);
	}
	return buffer;
}
direccionServidor levantarDeConfiguracion(char* nombreIp, char* nombrePuerto, char* rutaArchivo){
	t_config* configuracion = config_create(rutaArchivo);
	direccionServidor direccion;
	if(nombreIp != NULL){
	direccion.ip = config_get_string_value(configuracion, nombreIp);
	}
	direccion.puerto = config_get_int_value(configuracion, nombrePuerto);
	return direccion;
}
