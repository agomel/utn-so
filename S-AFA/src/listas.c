#include "listas.h"

DTB* obtenerDTBDeCola(int idDTB){
	DTB* dtb;
	int index = 0;
	for(int index = 0; index < listaDeTodosLosDTBs->elements_count; index++){
		waitMutex(&mutexListaDTBs);
		dtb = list_get(listaDeTodosLosDTBs, index);
		signalMutex(&mutexListaDTBs);
		if(dtb->id == idDTB && dtb->flag != 0){
			break;
		}
	}
	return dtb;
}

DTB* obtenerDTBDeColaRemoviendolo(int idDTB){
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
	printf("tamanio nueva lista de estado %c: %d\n",estado,lista->elements_count);
	return lista;
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
