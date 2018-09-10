#include "utilidades.h"

void* asignarMemoria(int cantidad){
	void* buffer = malloc(cantidad);
	if(buffer == NULL){
		printf("No hay espacio\n");
		exit(-99);
	}
	return buffer;
}

void deserializar(char** parametros,int emisor){
	int espacioALeer;

	int parametro=0;
	char* buffer;

	buffer=asignarMemoria(3);
	recibirMensaje(emisor, &buffer, 2);
	espacioALeer = atoi(buffer);
	while(espacioALeer != 99){
		parametros[parametro]=asignarMemoria(espacioALeer +1);
		recibirMensaje(emisor, &parametros[parametro], espacioALeer);

		free(buffer);
		buffer=asignarMemoria(3);
		recibirMensaje(emisor, &buffer, 2);
		espacioALeer = atoi(buffer);
		parametro++;
	}
}

direccionServidor levantarDeConfiguracion(char* nombreIp, char* nombrePuerto, char* rutaArchivo){
	t_config* configuracion = config_create(rutaArchivo);
	direccionServidor direccion;
	direccion.ip = config_get_string_value(configuracion, nombreIp);
	direccion.puerto = config_get_int_value(configuracion, nombrePuerto);
	return direccion;
}
