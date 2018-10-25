#include "consola.h"


char* concatenar(char* str1, char* str2){
	char* resultado = asignarMemoria(strlen(str1) + strlen(str2)+1);
	memcpy(resultado, str1, strlen(str1));
	memcpy(resultado + strlen(str1), str2, strlen(str2)+1);
	return resultado;
}

void consolita(){

	//ir al punto de montaje
	char* puntoDeMontaje = concatenar(".", PUNTO_MONTAJE);
	cd(puntoDeMontaje);
	free(puntoDeMontaje);

	//escuchar la consola
	while(1){
		char* mensajeReadLine = concatenar(PUNTO_MONTAJE, "$ ");
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
			printf("Pusiste md5 con el parametro %s \n", parametro);
		}
		else if(strcmp(comando, "cat") == 0){
			cat(parametro);
		}else{
			printf("No te entiendo man \n");
		}
	}
}



