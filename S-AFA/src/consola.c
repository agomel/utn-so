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

void consola(){
	while(1){
		char mensaje[1000];
		scanf("%s", mensaje);
		if(estado == CORRUPTO){
			log_error(logger, "Estado corrupto: No se reciben mensajes");
		}else{

			comandoCompleto cadenaArmada=rearmarCadena(mensaje);
			int comando=obtenerComando(cadenaArmada.comando);

			switch(comando){
				case SALIR:
					log_info(logger, "Comando salir");
					exit(1);
					break;
				case STATUS:
					log_info(logger, "Comando status");
					break;
				case FINALIZAR:
					log_info(logger, "Comando finalizar");
					pasarDTBAExit(cadenaArmada.parametro);
					break;
				case METRICAS:
					log_info(logger, "Comando metricas");
					break;
				case EJECUTAR:
					log_info(logger, "Comando ejecutar");
					ponerProcesoEnNew(cadenaArmada.parametro);
					break;
				default:
					log_error(logger, "Comando erroneo");
			}
		}
	}
}
