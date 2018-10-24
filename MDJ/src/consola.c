#include "consola.h"


void consolita(){
	struct dirent* pDirent;
	DIR* pdir = opendir(PUNTO_MONTAJE);
	if(pdir == NULL){
		printf("No puedo abrir el directorio %s \n", PUNTO_MONTAJE);
	}
	while(1){
		char* texto = readline(PUNTO_MONTAJE);
		//get comando
		char* comando = strtok(texto, " ");
		char* parametro = strtok(NULL, " ");

		//entender el comando
		if(strcmp(comando, "ls") == 0){
			if(parametro != NULL){
				printf("Pusiste ls con el parametro %s \n", parametro);
			}else{
				printf("Pusiste ls sin ningun parametro \n");
				while((pDirent = readdir(pdir)) != NULL){
					printf("Archivo %s \n", pDirent->d_name);
				}
			}
		}else if(strcmp(comando, "cd") == 0){
			printf("Pusiste cd con el parametro %s \n", parametro);

			realloc(PUNTO_MONTAJE, strlen(PUNTO_MONTAJE) + strlen(parametro)+1);

			memcpy(PUNTO_MONTAJE + strlen(PUNTO_MONTAJE), parametro, strlen(parametro)+1);

		}else if(strcmp(comando, "md5") == 0){
			printf("Pusiste md5 con el parametro %s \n", parametro);
		}else if(strcmp(comando, "cat") == 0){
			printf("Pusiste cat con el parametro %s \n", parametro);
			FILE* archivo = fopen(parametro, "r");
			char* linea = asignarMemoria(250);
			while(fscanf(archivo, "%s", linea) != EOF){
				printf("%s\n", linea);
			}
			fclose(archivo);

		}else if(strcmp(comando, "exit") == 0){
			printf("Chauuuu \n");
			break;
		}else{
			printf("No te entiendo man \n");
		}
	}
	closedir(pdir);
}




