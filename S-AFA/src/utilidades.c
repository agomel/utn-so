#include "utilidades.h"
void* asignarMemoria(int cantidad){
	void* buffer = malloc(cantidad);
	if(buffer == NULL){
		printf("No hay espacio");
		return 1;
	}
	return buffer;
}
