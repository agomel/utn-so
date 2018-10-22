#include "consola.h"


void consolita(){
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

				struct dirent* pDirent;
				DIR* pdir = opendir("/mnt/FIFA_FS/");
				if(pdir != NULL){
					printf("ago\n");
					while((pDirent = readdir(pdir)) != NULL){
						printf("Archivo %s \n", pDirent->d_name);
					}
					closedir(pdir);
				}else{
					printf("No puedo abrir el directorio %s \n", PUNTO_MONTAJE);
				}



			}
		}else if(strcmp(comando, "cd") == 0){
			printf("Pusiste cd con el parametro %s \n", parametro);
		}else if(strcmp(comando, "md5") == 0){
			printf("Pusiste md5 con el parametro %s \n", parametro);
		}else if(strcmp(comando, "cat") == 0){
			printf("Pusiste cat con el parametro %s \n", parametro);
		}else if(strcmp(comando, "exit") == 0){
			printf("Chauuuu \n");
			break;
		}else{
			printf("No te entiendo man \n");
		}


	}
}
