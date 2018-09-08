#include "utilidades.h"
void* asignarMemoria(int cantidad){
	void* buffer = malloc(cantidad);
	if(buffer == NULL){
		printf("No hay espacio");
		exit(-99);
	}
	return buffer;
}
