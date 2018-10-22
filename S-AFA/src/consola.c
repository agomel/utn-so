#include "consola.h"

int obtenerComando(char* ingresado){
	int comando=6;
	const char* comandos[] = {"salir","status","finalizar","metricas","ejecutar"};
	for(int i = 0;i<5;i++){
		if(strcmp(ingresado, comandos[i]) == 0) comando=i;
	}
	return comando;
}


void consola(){
	while(1){

		char* mensaje = readline("");


	    char* comandoIngresado =  strtok(mensaje, " ");
	    char* parametro =  strtok(NULL, " ");

		if(estado == CORRUPTO){
			log_error(logger, "Estado corrupto: No se reciben mensajes");
		}else{

			int comando=obtenerComando(comandoIngresado);
			int idDTB;
			switch(comando){
				case SALIR:
					log_info(logger, "Comando salir");
					exit(1);
					break;
				case STATUS:
					log_info(logger, "Comando status");
					if(parametro == NULL){
						mostrarStatus();
					}else{
						idDTB = atoi(parametro);
						DTB* dtb = obtenerDTBDeCola(idDTB);
						loguearEstadoDTB(dtb);
					}
					break;
				case FINALIZAR:
					idDTB = atoi(parametro);
					log_info(logger, "Comando finalizar");
					DTB* dtb = obtenerDTBDeCola(idDTB);
					if(!dtb){
						log_error(logger, "No existe DTB con id %d", idDTB);
					}else if(dtb->estado == EXECUTE){
						waitMutex(&mutexEjecutandoCPU);
						int socketCPU = dictionary_get(ejecutandoCPU, intToString(idDTB));
						signalMutex(&mutexEjecutandoCPU);

						waitMutex(&mutexCpusAFinalizarDTBs);
						dictionary_put(cpusAFinalizarDTBs, intToString(idDTB), socketCPU);
						signalMutex(&mutexCpusAFinalizarDTBs);
					}else{
						pasarDTBAExit(idDTB);
					}
					break;
				case METRICAS:
					log_info(logger, "Comando metricas");
					if(parametro == NULL){
						mostrarMetricas();
					}else{
						idDTB = atoi(parametro);
						mostrarMetricasConDTB(idDTB);
					}
					break;
				case EJECUTAR:
					log_info(logger, "Comando ejecutar");
					ponerProcesoEnNew(parametro);
					break;
				default:
					log_error(logger, "Comando erroneo");
			}
		}
	}
}
