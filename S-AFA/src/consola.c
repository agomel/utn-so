#include "consola.h"
#include <biblioteca/dtb.h>

void loguearEstadoDTB(DTB* dtb){
	log_info(logger, "id: %d, escriptiorio: %s, programCounter: %d, status: %s, quantum: %d, flag: %d",
			dtb->id, dtb->escriptorio, dtb->programCounter, nombreEstado(dtb->estado), dtb->quantum, dtb->flag);
}

void loguearEstadoDeLista(t_list* lista, char estado){
	log_info(logger, "__________________________________");
	log_info(logger, "Estado de lista: %s", nombreEstado(estado));
	log_info(logger, "Cantidad de DTBs en lista: %d", lista->elements_count);
	if(lista->elements_count > 0) log_info(logger, "DTBs en lista:");
	for(int i = 0; i<lista->elements_count; i++){
		loguearEstadoDTB(list_get(lista, i));
	}
	list_destroy(lista);
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
			int idDTB;
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
					idDTB = atoi(cadenaArmada.parametro);
					log_info(logger, "Comando finalizar");
					DTB* dtb = obtenerDTBDeCola(idDTB);
					if(dtb->estado == EXECUTE){

						waitMutex(&mutexEjecutandoCPU);
						int socketCPU = dictionary_get(ejecutandoCPU, cadenaArmada.parametro);
						signalMutex(&mutexEjecutandoCPU);

						waitMutex(&mutexCpusAFinalizarDTBs);
						dictionary_put(cpusAFinalizarDTBs, intToString(socketCPU), idDTB);
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
