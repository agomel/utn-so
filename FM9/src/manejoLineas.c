#include "FM9.h"

bool esUltimoCaracter(char caracter) {
		return caracter == '\0';
}

int cantidadDeLineas(char* texto){
	int contador = 1;
	for(int i = 0; i < strlen(texto); i++){
		if(esUltimoCaracter(texto[i]))
			break;

		if(texto[i] == '\n')
			contador++;
	}

	return contador;
}
