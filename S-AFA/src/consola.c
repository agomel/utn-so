#include "consola.h"
#include <biblioteca/dtb.h>

void loguearEstadoDeLista(t_list* lista, char estado){
	log_info(logger, "Estado de lista: %c", estado);
	log_info(logger, "Cantidad de DTBs en lista: %d", lista->elements_count);
	log_info(logger, "Ids de DTBs en lista:");
	for(int i = 0; i<lista->elements_count; i++){
		log_info(logger, "d", list_get(lista, i));
	}
}
void crearStatusDeMetricas(){
	t_list* listaNews = filtrarListaPorEstado(NEW);
	loguearEstadoDeLista(listaNews, NEW);
	t_list* listaReady = filtrarListaPorEstado(READY);
	loguearEstadoDeLista(listaReady, READY);
	t_list* listaBlocked = filtrarListaPorEstado(BLOCKED);
	loguearEstadoDeLista(listaBlocked, BLOCKED);
	t_list* listaExit = filtrarListaPorEstado(EXIT);
	loguearEstadoDeLista(listaExit, EXIT);
	t_list* listaExecute = filtrarListaPorEstado(EXECUTE);
	loguearEstadoDeLista(listaExecute, EXECUTE);
	t_list* listaReadyPrioridad = filtrarListaPorEstado(READY_PRIORIDAD);
	loguearEstadoDeLista(listaReadyPrioridad, READY_PRIORIDAD);
}

int obtenerComando(char*ingresado){
	int comando=6;
	const char* comandos[] = {"salir","status","finalizar","metricas","ejecutar"};
	for(int i;i<5;i++){
		if(strcmp(ingresado, comandos[i]) == 0) comando=i;
	}
	return comando;
}

comandoCompleto rearmarCadena(char* cadenaIngresada){
		char* cadena[2];
	    char* token = strtok(cadenaIngresada, ",");
	    int posicion=0;
	    comandoCompleto comando;
	    cadena[1]=NULL;
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

			switch(comando){
				case SALIR:
					log_info(logger, "Comando salir");
					exit(1);
					break;
				case STATUS:
					log_info(logger, "Comando status");
					crearStatusDeMetricas();
					break;
				case FINALIZAR:
					log_info(logger, "Comando finalizar");
					DTB* dtb = obtenerDTBDeCola(cadenaArmada.parametro);
					if(dtb->estado = EXECUTE){

						waitMutex(&mutexSocketsCPus);
						int socketCPU = dictionary_get(socketsCPUs, intToString(cadenaArmada.parametro));
						signalMutex(&mutexSocketsCPus);

						waitMutex(&mutexCpusAFinalizarDTBs);
						dictionary_put(cpusAFinalizarDTBs, intToString(socketCPU),cadenaArmada.parametro);
						signalMutex(&mutexCpusAFinalizarDTBs);
					}else{
						pasarDTBAExitGuardandoNuevo(dtb);
					}
					break;
				case METRICAS:
					log_info(logger, "Comando metricas");
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
