#include "consola.h"
int obtenerComando(char*ingresado){
	int comando=6;
	const char* comandos[] = {"salir","status","finalizar","metricas","ejecutar"};
	for(int i;i<5;i++){
		if(strcmp(ingresado, comandos[i]) == 0) comando=i;
	}
	return comando;

}
comandoCompleto rearmarCadena(char* cadenaIngresada){
		char* cadena[2];
	    char* token = strtok(cadenaIngresada, ",");
	    int posicion=0;
	    comandoCompleto comando;
	    cadena[1]=NULL;
	    while (token != NULL)
	    {
	        cadena[posicion]=token;
	        token = strtok(NULL, ",");
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
			default:perror("no entendes los comandos\n");

		}

	}
}
