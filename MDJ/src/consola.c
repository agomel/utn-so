#include "consola.h"


char* concatenar(char* str1, char* str2){
	char* resultado = asignarMemoria(strlen(str1) + strlen(str2)+1);
	memcpy(resultado, str1, strlen(str1));
	memcpy(resultado + strlen(str1), str2, strlen(str2)+1);
	return resultado;
}

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
	chdir(directorio);
	char* path = malloc(250);
	getcwd(path, 250);
	memcpy(PUNTO_MONTAJE, path, strlen(path)+1);
	free(path);
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
			FILE* archivo = fopen(parametro, "r");
			char* linea = asignarMemoria(250);
			while(fscanf(archivo, "%s", linea) != EOF){
				printf("%s\n", linea);
			}
			fclose(archivo);

		}else{
			printf("No te entiendo man \n");
		}
	}
}



