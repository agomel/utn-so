#include "consola.h"
#include <biblioteca/dtb.h>
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
//ESTO VA EN PLP
void ponerProcesoEnNew(char* escriptorio){
	DTB proceso = crearDTB(escriptorio);
	//TODO wait mutexnew
	//TODO agregar a cola new
	//TODO signal mutexnew
}

void realizarProcesoDummy(DTB proceso){

}
//TERMINA PLP
void consola(){

	while(1){
		char mensaje[1000];
		scanf("%s", mensaje);

		comandoCompleto cadenaArmada=rearmarCadena(mensaje);
		int comando=obtenerComando(cadenaArmada.comando);


		switch(comando){
			case SALIR: printf("salir\n"); exit(1);
				break;
			case STATUS: printf("seleccionaste status\n");
				break;
			case FINALIZAR: printf("seleccionaste finalizar\n");
				break;
			case METRICAS: printf("seleccionaste metricas\n");
				break;
			case EJECUTAR:
				printf("seleccionaste ejecutar\n");
				ponerProcesoEnNew(cadenaArmada.parametro);
				break;
			default:perror("no entendes los comandos\n");

		}

	}
}
