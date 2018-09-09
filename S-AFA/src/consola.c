#include "consola.h"
int obtenerComando(char*ingresado){
	int comando=0;
	const char* comandos[] = {"salir","status","finalizar","metricas","ejecutar"};
	for(comando;comando<=5;comando++){
		if(strcmp(ingresado, comandos[comando]) == 0) break;
	}
	return comando;

}
comandoCompleto rearmarCadena(char* cadenaIngresada){
	    // Returns first token
		char* cadena[2];
	    char* token = strtok(cadenaIngresada, ",");
	    int posicion=0;
	    // Keep printing tokens while one of the
	    // delimiters present in str[].
	    comandoCompleto comando;
	    cadena[1]=NULL;
	    while (token != NULL)
	    {
	        printf("%s\n", token);
	        cadena[posicion]=token;
	        token = strtok(NULL, "-");
	        posicion++;
	    }
	    comando.comando=cadena[0];
	    comando.parametro=cadena[1];
	    return comando;
}

void consola(){

	while(1){
		char mensaje[1000];
		scanf("%s", mensaje);

		comandoCompleto cadenaArmada=rearmarCadena(mensaje);
		int comando=obtenerComando(cadenaArmada.comando);


		switch(comando){
			case salir: printf("salir\n"); exit(1);
				break;
			case status: printf("seleccionaste status\n");
				break;
			case finalizar: printf("seleccionaste finalizar\n");
				break;
			case metricas: printf("seleccionaste metricas\n");
				break;
			case ejecutar: printf("seleccionaste ejecutar\n");
				break;
			default:printf("no entendes los comandos\n");
				break;

		}

	}
}
