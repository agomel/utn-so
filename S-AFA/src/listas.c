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
	waitMutex(&listaDeTodosLosDTBs);
	t_list* lista = list_filter(listaDeTodosLosDTBs, estaEnEstado);
	signalMutex(&listaDeTodosLosDTBs);
	return lista;
}

DTB* obtenerPrimerDTBEnNew(){
	t_list* lista = filtrarListaPorEstado(NEW);
	return list_get(lista, 0);
}

void agregarDTBALista(DTB* dtb){
	waitMutex(&listaDeTodosLosDTBs);
	list_add(listaDeTodosLosDTBs, dtb);
	signalMutex(&listaDeTodosLosDTBs);
}

DTB* removerDTBPorIndice(int indice){
	waitMutex(&listaDeTodosLosDTBs);
	DTB* dtb = list_remove(listaDeTodosLosDTBs, 0);
	signalMutex(&listaDeTodosLosDTBs);
	return dtb;
}
