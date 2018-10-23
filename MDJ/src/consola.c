#include "consola.h"


void consolita(){

	//ir al punto de montaje
	char* montajeActual = concatenar(".", MONTAJE_ACTUAL);
	cd(montajeActual);
	free(montajeActual);

	//escuchar la consola
	while(1){
		char* mensajeReadLine = concatenar(MONTAJE_ACTUAL, "$ ");
		char* texto = readline(mensajeReadLine);
		free(mensajeReadLine);

		//get comando
		char* comando = strtok(texto, " ");
		char* parametro = strtok(NULL, " ");

		if(strcmp(comando, "ls") == 0){
			if(parametro == NULL){
				ls(".");
			}else{
				ls(parametro);
			}
		}
		else if(strcmp(comando, "cd") == 0){
			cd(parametro);
		}
		else if(strcmp(comando, "md5") == 0){
			md5(parametro);
		}
		else if(strcmp(comando, "cat") == 0){
			cat(parametro);
		}else{
			printf("No te entiendo man \n");
		}
	}
}



