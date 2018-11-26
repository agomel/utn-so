#include "listas.h"

DTB* obtenerDTBDeCola(int idDTB){
	DTB* dtb;
	int loEncontro = 0;
	int index = 0;
	waitMutex(&mutexListaDTBs);
	for(int index = 0; index < listaDeTodosLosDTBs->elements_count; index++){
		dtb = list_get(listaDeTodosLosDTBs, index);
		if(dtb->id == idDTB && dtb->flag != 0){
			loEncontro = 1;
			break;
		}
	}
	signalMutex(&mutexListaDTBs);
	if(!loEncontro)
		return NULL;

	return dtb;
}

DTB* obtenerDTBDeColaRemoviendolo(int idDTB){
	DTB* dtb;
	int loEncontro = 0;
	int index = 0;
	waitMutex(&mutexListaDTBs);
	for(int index = 0; index < listaDeTodosLosDTBs->elements_count; index++){
		dtb = list_get(listaDeTodosLosDTBs, index);
		if(dtb->id == idDTB && dtb->flag != 0){
			list_remove(listaDeTodosLosDTBs, index);
			loEncontro = 1;
			break;
		}
	}
	signalMutex(&mutexListaDTBs);
	if(!loEncontro)
		return NULL;

	return dtb;
}

void removerDTBDeCola(int idDTB){
	DTB* dtb;
	int index = 0;
	waitMutex(&mutexListaDTBs);
	for(int index = 0; index < listaDeTodosLosDTBs->elements_count; index++){
		dtb = list_get(listaDeTodosLosDTBs, index);
		if(dtb->id == idDTB && dtb->flag != 0){
			list_remove(listaDeTodosLosDTBs, index);
			freeDTBSAFA(dtb);
			break;
		}
	}
	signalMutex(&mutexListaDTBs);
}
DTB* obtenerDummyDeColaRemoviendolo(){
	DTB* dtb;
	int index = 0;

	waitMutex(&mutexListaDTBs);
	for(int index = 0; index < listaDeTodosLosDTBs->elements_count; index++){
		dtb = list_get(listaDeTodosLosDTBs, index);
		if(dtb->flag == 0){
			list_remove(listaDeTodosLosDTBs, index);
			break;
		}
	}
	signalMutex(&mutexListaDTBs);
	return dtb;
}

t_list* filtrarListaPorEstado(char estado){
	bool estaEnEstado(DTB* dtb){
		return (dtb->estado == estado);
	}
	waitMutex(&mutexListaDTBs);
	t_list* lista = list_filter(listaDeTodosLosDTBs, estaEnEstado);
	signalMutex(&mutexListaDTBs);
	return lista;
}

t_list* filtrarListaPorDTBsConArchivosAbiertos(){
	bool tieneArchivosAbiertos(DTB* dtb){
		return (dtb->tamanioArchivosAbiertos > 0);
	}
	waitMutex(&mutexListaDTBs);
	t_list* lista = list_filter(listaDeTodosLosDTBs, tieneArchivosAbiertos);
	signalMutex(&mutexListaDTBs);
	return lista;
}

DTB* obtenerDTBConArchivoMasGrande(){
	bool tieneArchivoMasGrande(DTB* dtb1, DTB* dtb2){
		return (dtb1->tamanioArchivosAbiertos > dtb2->tamanioArchivosAbiertos);
	}
	t_list* lista = filtrarListaPorEstado(READY);
	list_sorted(lista, tieneArchivoMasGrande);
	DTB* dtb = list_get(lista, 0);
	list_destroy(lista);
	return dtb;
}
void agregarDTBALista(DTB* dtb){
	waitMutex(&mutexListaDTBs);
	list_add(listaDeTodosLosDTBs, dtb);
	signalMutex(&mutexListaDTBs);
}

void pasarDTBAlFinalDeListaDesdeNew(int idDTB){
	DTB* dtb = obtenerDTBDeColaRemoviendolo(idDTB);
	dtb->estado = MANDADO_A_DUMIZZAR;
	agregarDTBALista(dtb);
}

DTB* obtenerPrimerDTBEnNew(){
	t_list* lista = filtrarListaPorEstado(NEW);
	DTB* dtb = list_get(lista, 0);
	pasarDTBAlFinalDeListaDesdeNew(dtb->id);
	return dtb;
}


DTB* removerDTBPorIndice(int indice){
	waitMutex(&mutexListaDTBs);
	DTB* dtb = list_remove(listaDeTodosLosDTBs, 0);
	signalMutex(&mutexListaDTBs);
	return dtb;
}

void cambiarEstadoDummy(char estado){
	waitMutex(&mutexDummy);
	DTB* dummy = obtenerDummyDeColaRemoviendolo();
	dummy->estado = estado;
	agregarDTBALista(dummy);
	signalMutex(&mutexDummy);
}
int dummyCargado(){
	bool esDummy(DTB* dtb){
		return dtb->flag == 0;
	}
	waitMutex(&mutexListaDTBs);
	int hayDummy = list_any_satisfy(listaDeTodosLosDTBs, esDummy);
	signalMutex(&mutexListaDTBs);
	return hayDummy;
}

void cambiarEstadoDummyCargandolo(DTB* dummy){
	if(dummyCargado()){
		obtenerDummyDeColaRemoviendolo();
	}
	agregarDTBALista(dummy);
}

int obtenerCPUDisponibleYOcupar(int id){
	bool estaDisponible(SocketCPU* socket){
		return (socket->ocupado == 0);
	}
	waitMutex(&mutexSocketsCPus);
	SocketCPU* socketCPU = list_find(socketsCPUs, estaDisponible);
	signalMutex(&mutexSocketsCPus);

	if(socketCPU != NULL && socketCPU->ocupado == 0){
		socketCPU->ocupado = 1;
	}else{
		log_error(logger, "No hay CPUs disponibles");
	}

	waitMutex(&mutexEjecutandoCPU);
	dictionary_put(ejecutandoCPU, intToString(id), socketCPU->socket);
	signalMutex(&mutexEjecutandoCPU);
	return socketCPU->socket;
}

void liberarCPU(int idSocket, int idDTB){
	bool obtenerCPU(SocketCPU* socket){
		return (socket->socket == idSocket);
	}
	waitMutex(&mutexSocketsCPus);
	SocketCPU* socketCPU = list_find(socketsCPUs, obtenerCPU);
	signalMutex(&mutexSocketsCPus);

	socketCPU->ocupado = 0;

	waitMutex(&mutexEjecutandoCPU);
	dictionary_remove(ejecutandoCPU, intToString(idDTB));
	signalMutex(&mutexEjecutandoCPU);

	signalSem(&gradoMultiprocesamiento);
}

int estaEnExit(int idDtb){
	DTB* dtb = obtenerDTBDeCola(idDtb);
	return dtb->estado == EXIT;
}

