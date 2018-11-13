#include "parser.h"

Metadata* obtenerMetadata(char* path){
	Metadata* metadata = asignarMemoria(sizeof(Metadata));
	t_config* configuracion = config_create(path);
	metadata->tamanio = config_get_int_value(configuracion, "TAMANIO");
	metadata->bloques = config_get_array_value(configuracion, "BLOQUES");
	config_destroy(configuracion);
	return metadata;
}

void freeMetadata(Metadata* metadata){
	int cantidadBloques = metadata->bloques / TAMANIO_BLOQUES + 1;
	for(int i = 0; i < cantidadBloques; i++){
		free(metadata->bloques[i]);
	}
	free(metadata->bloques);
	free(metadata);
}
