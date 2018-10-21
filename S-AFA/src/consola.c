#include "consola.h"

int obtenerComando(char*ingresado){
	int comando=6;
	const char* comandos[] = {"salir","status","finalizar","metricas","ejecutar"};
	for(int i = 0;i<5;i++){
		if(strcmp(ingresado, comandos[i]) == 0) comando=i;
	}
	return comando;
}

comandoCompleto rearmarCadena(char* cadenaIngresada){
		char* cadena[2];
	    char* token = strtok(cadenaIngresada, ",");
	    int posicion=0;
	    comandoCompleto comando;
	    cadena[1] = NULL;
	    while (token != NULL)
	    {
	        cadena[posicion]=token;
	        token = strtok(NULL, ",");
	        posicion++;
	    }
	    comando.comando=cadena[0];
	    comando.parametro=cadena[1];
	    return comando;
}

void consola(){
	while(1){
		char mensaje[1000];
		scanf("%s", mensaje);
		if(estado == CORRUPTO){
			log_error(logger, "Estado corrupto: No se reciben mensajes");
		}else{

			comandoCompleto cadenaArmada=rearmarCadena(mensaje);
			int comando=obtenerComando(cadenaArmada.comando);
			int idDTB;
			switch(comando){
				case SALIR:
					log_info(logger, "Comando salir");
					exit(1);
					break;
				case STATUS:
					log_info(logger, "Comando status");
					if(cadenaArmada.parametro == NULL){
						mostrarStatus();
					}else{
						idDTB = atoi(cadenaArmada.parametro);
						DTB* dtb = obtenerDTBDeCola(idDTB);
						loguearEstadoDTB(dtb);
					}
					break;
				case FINALIZAR:
					idDTB = atoi(cadenaArmada.parametro);
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
					if(cadenaArmada.parametro == NULL){
						mostrarMetricas();
					}else{
						idDTB = atoi(cadenaArmada.parametro);
						mostrarMetricasConDTB(idDTB);
					}
					break;
				case EJECUTAR:
					log_info(logger, "Comando ejecutar");
					ponerProcesoEnNew(cadenaArmada.parametro);
					break;
				default:
					log_error(logger, "Comando erroneo");
			}
		}
	}
}
