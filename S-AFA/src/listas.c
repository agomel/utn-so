#include "listas.h"

DTB* obtenerDTBDeCola(int idDTB){
	DTB* dtb;
	int loEncontro = 0;
	int index = 0;
	for(int index = 0; index < listaDeTodosLosDTBs->elements_count; index++){
		waitMutex(&mutexListaDTBs);
		dtb = list_get(listaDeTodosLosDTBs, index);
		signalMutex(&mutexListaDTBs);
		if(dtb->id == idDTB && dtb->flag != 0){
			loEncontro = 1;
			break;
		}
	}
	if(!loEncontro)
		return NULL;

	return dtb;
}

DTB* obtenerDTBDeColaRemoviendolo(int idDTB){
	DTB* dtb;
	int loEncontro = 0;
	int index = 0;
	for(int index = 0; index < listaDeTodosLosDTBs->elements_count; index++){
		waitMutex(&mutexListaDTBs);
		dtb = list_get(listaDeTodosLosDTBs, index);
		signalMutex(&mutexListaDTBs);
		if(dtb->id == idDTB && dtb->flag != 0){
			waitMutex(&mutexListaDTBs);
			list_remove(listaDeTodosLosDTBs, index);
			signalMutex(&mutexListaDTBs);
			loEncontro = 1;
			break;
		}
	}

	if(!loEncontro)
		return NULL;

	return dtb;
}

void removerDTBDeCola(int idDTB){
	DTB* dtb;
	int index = 0;
	for(int index = 0; index < listaDeTodosLosDTBs->elements_count; index++){
		waitMutex(&mutexListaDTBs);
		dtb = list_get(listaDeTodosLosDTBs, index);
		signalMutex(&mutexListaDTBs);
		if(dtb->id == idDTB && dtb->flag != 0){
			waitMutex(&mutexListaDTBs);
			list_remove(listaDeTodosLosDTBs, index);
			signalMutex(&mutexListaDTBs);
			freeDTBSAFA(dtb);
			break;
		}
	}
}
DTB* obtenerDummyDeColaRemoviendolo(){
	DTB* dtb;
	int index = 0;
	for(int index = 0; index < listaDeTodosLosDTBs->elements_count; index++){
		waitMutex(&mutexListaDTBs);
		dtb = list_get(listaDeTodosLosDTBs, index);
		signalMutex(&mutexListaDTBs);
		if(dtb->flag == 0){
			waitMutex(&mutexListaDTBs);
			list_remove(listaDeTodosLosDTBs, index);
			signalMutex(&mutexListaDTBs);
			break;
		}
	}
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
	waitMutex(&mutexListaDTBs);
	t_list* lista = list_sorted(listaDeTodosLosDTBs, tieneArchivoMasGrande);
	DTB* dtb = list_get(lista, 0);
	signalMutex(&mutexListaDTBs);
	list_destroy(lista);
	return dtb;
}
void agregarDTBALista(DTB* dtb){
	waitMutex(&mutexListaDTBs);
	list_add(listaDeTodosLosDTBs, dtb);
	signalMutex(&mutexListaDTBs);
}

void pasarDTBAlFinalDeLista(int idDTB){
	DTB* dtb = obtenerDTBDeColaRemoviendolo(idDTB);
	agregarDTBALista(dtb);
}

DTB* obtenerPrimerDTBEnNew(){
	t_list* lista = filtrarListaPorEstado(NEW);
	DTB* dtb = list_get(lista, 0);
	pasarDTBAlFinalDeLista(dtb->id);
	return dtb;
}


DTB* removerDTBPorIndice(int indice){
	waitMutex(&mutexListaDTBs);
	DTB* dtb = list_remove(listaDeTodosLosDTBs, 0);
	signalMutex(&mutexListaDTBs);
	return dtb;
}

cambiarEstadoDummy(char estado){
	DTB* dummy = obtenerDummyDeColaRemoviendolo();
	dummy->estado = estado;
	agregarDTBALista(dummy);
}

int obtenerCPUDisponibleYOcupar(int id){

	bool estaDisponible(SocketCPU* socket){
		return (socket->ocupado == 0);
	}
	waitMutex(&mutexSocketsCPus);
	SocketCPU* socketCPU = list_find(socketsCPUs, estaDisponible);
	signalMutex(&mutexSocketsCPus);

	if(socketCPU->ocupado == 0){
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
