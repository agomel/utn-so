#include "FM9.h"

bool esUltimoCaracter(char caracter) {
		return caracter == '\0';
}

int cantidadDeLineas(char* texto){
	int contador = 0;
	for(int i = 0; i < strlen(texto); i++){
		if(esUltimoCaracter(texto[i]))
			break;

		if(texto[i] == '\n')
			contador++;
	}

	return contador;
}

void freeLineasBasura(char** lineaSinBasura, char* lineaConBasura){
	free(lineaConBasura);
	free(lineaSinBasura[0]);
	free(lineaSinBasura[1]);
	free(lineaSinBasura);
}

void freeLineas(char** lineas){
	int contador = 0;
	int entro = 0;
	while(lineas[contador] != NULL && lineas[contador][0] != '\n'){
		entro = 1;
		free(lineas[contador]);
		contador++;
	}

	if(entro)
		free(lineas);
}

char** dividirPorLineas(char* datos){
	int lineas = cantidadDeLineas(datos);
	char** vector = malloc(sizeof(char*) * lineas);
	char* resto = string_duplicate(datos);

	for(int i = 0; i < lineas; i++){
		char* linea = string_new();
		char* caracter = string_repeat(resto[0], 1);
		string_append(&linea, caracter);
		while(*caracter != '\n'){
			resto = string_substring_from(resto, 1);
			caracter = string_repeat(resto[0], 1);
			string_append(&linea, caracter);

			if(*caracter == '\n')
				resto = string_substring_from(resto, 1);
		}

		if(linea[0] == '\n')
			vector[i] = NULL;
		else
			vector[i] = string_duplicate(linea);

		free(linea);
		free(caracter);
	}

return vector;
}
