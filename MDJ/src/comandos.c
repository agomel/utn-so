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
			memcpy(MONTAJE_ACTUAL, path, strlen(path)+1);
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

void md5(char* parametro){
	FILE* archivo = fopen(parametro, "r");
	char* linea = asignarMemoria(250);
	while(fscanf(archivo, "%s", linea) != EOF){
		unsigned char digest[16];
		MD5_CTX context;
		MD5_Init(&context);
		MD5_Update(&context, linea, strlen(linea) + 1);
		MD5_Final(digest, &context);

		char out[33];
		for(int i = 0; i < 16; ++i){
			sprintf(&out[i*2], "%02x", (unsigned int)digest[i]);
		}
		printf("%s\n", out);
	}
	free(linea);
	fclose(archivo);
}


