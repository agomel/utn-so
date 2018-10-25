#include "comandos.h"



void ls(char* directorio){
	struct dirent* pDirent;
	DIR* pdir = opendir(directorio);
	if(pdir == NULL){
		printf("No puedo abrir el directorio %s \n", directorio);
	}else{
		while((pDirent = readdir(pdir)) != NULL){
			printf("%s \n", pDirent->d_name);
		}
		closedir(pdir);
	}
}


void cd(char* directorio){
	if(chdir(directorio) == 0){
		char* path = malloc(250);
			getcwd(path, 250);
			memcpy(PUNTO_MONTAJE, path, strlen(path)+1);
			free(path);
	}else{
		printf("Error, no pude entrar a ese path\n");
	}
}

void cat(char* parametro){
	FILE* archivo = fopen(parametro, "r");
	char* linea = asignarMemoria(250);
	while(fscanf(archivo, "%s", linea) != EOF){
		printf("%s\n", linea);
	}
	free(linea);
	fclose(archivo);
}
