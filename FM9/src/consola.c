#include "consola.h"

void consola(){
	//escuchar la consola
	while(1){
		char* mensaje = readline("");
		char* comando =  strtok(mensaje, " ");
		char* parametro =  strtok(NULL, " ");

		if(strcmp(comando, "dump") == 0){
			int idDTB = atoi(parametro);
			if(strcmp(modo, "SEG") == 0)
				dumpSegPura(idDTB);

			if(strcmp(modo, "SPA") == 0)
				dumpSegPag(idDTB);

			if(strcmp(modo, "TPI") == 0)
				dumpInvertida(idDTB);

		}else if(strcmp(comando, "exit") == 0){
			exit(200);
		}else{
			printf("Comando incorrecto\n");
		}
	}
}


