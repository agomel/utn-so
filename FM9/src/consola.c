#include "consola.h"

void consola(){

	//escuchar la consola
	while(1){
		char* mensaje = readline("");
		char* comando =  strtok(mensaje, " ");
		char* parametro =  strtok(NULL, " ");
		int idDTB = atoi(parametro);

		if(strcmp(comando, "dump") == 0){
			if(strcmp(modo, "SEG_PURA") == 0)
				dumpSegPura(idDTB);

			if(strcmp(modo, "SEG_PAG") == 0)
				dumpSegPag(idDTB);

			if(strcmp(modo, "INV") == 0)
				dumpInvertida(idDTB);

		}else if(strcmp(comando, "exit") == 0){
			exit(200);
		}else{
			printf("Comando incorrecto\n");
		}
	}
}

void dumpSegPag(idDTB){

}

void dumpInvertida(idDTB){

}
