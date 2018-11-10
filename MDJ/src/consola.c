#include "consola.h"


void consolita(){

	//ir al punto de montaje
	char* montajeActual = asignarMemoria(sizeof(char)); //No hace falta hacer el malloc para todo, se hace el realloc en la funcion
	memcpy(montajeActual, ".", sizeof(char));
	string_append(&montajeActual, MONTAJE_ACTUAL);
	cd(montajeActual);
	free(montajeActual);

	//escuchar la consola
	while(1){

		char* mensajeReadLine = asignarMemoria(strlen(MONTAJE_ACTUAL) + 1);
		memcpy(mensajeReadLine, MONTAJE_ACTUAL, strlen(MONTAJE_ACTUAL) +1);
		string_append(&mensajeReadLine, "$ ");
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



