#include "parser.h"

Metadata* obtenerMetadata(char* path){
	Metadata* metadata = asignarMemoria(sizeof(Metadata));
	t_config* configuracion = config_create(path);
	RETARDO = config_get_int_value(configuracion, "RETARDO");
	char* punteroPuntoMontaje = config_get_string_value(configuracion, "PUNTO_MONTAJE");
	MONTAJE_ACTUAL = asignarMemoria(250);
	memcpy(MONTAJE_ACTUAL, punteroPuntoMontaje, strlen(punteroPuntoMontaje) + 1);
	free(punteroPuntoMontaje);

	return metadata;
}
