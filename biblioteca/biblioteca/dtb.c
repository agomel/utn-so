#include "dtb.h"

int obtenerId(){
	waitMutex(&mutexIdsDTB);
	int id = contadorIds;
	contadorIds++;
	signalMutex(&mutexIdsDTB);
	return id;
}

DTB crearDTB (char* parametro){
	DTB dtb;
	dtb.escriptorio = parametro;
	dtb.flag = 1;
	dtb.id = obtenerId();
	dtb.listaDeArchivos = list_create();
	//TODO poner valor valido en programCounter
	dtb.programCounter = 0;
	dtb.estado = NEW;
	dtb.quantum = 0;
	return dtb;
}

void serializarYEnviarDTB(int receptor, DTB dtb, t_log* logger, char operacion){
	//Asigno tamanio al buffer
	log_debug(logger, "Enviando dtb con id: %d", dtb.id);
	int tamanioEscriptorio = strlen(dtb.escriptorio) + 1;
	int tamanioBuffer = sizeof(char) + sizeof(int) + tamanioEscriptorio + sizeof(int)*3 + obtenerTamanioListaStrings(dtb.listaDeArchivos)
			+ sizeof(int) + sizeof(char);
	void* buffer = asignarMemoria(tamanioBuffer);

	//Lleno el buffer
	int desplazamiento = 0;

	concatenarChar(buffer, &desplazamiento, operacion);
	concatenarString(buffer, &desplazamiento, dtb.escriptorio);
	concatenarInt(buffer, &desplazamiento, dtb.flag);
	concatenarInt(buffer, &desplazamiento, dtb.id);
	concatenarInt(buffer, &desplazamiento, dtb.programCounter);
	concatenarListaString(buffer, &desplazamiento, dtb.listaDeArchivos);
	concatenarInt(buffer, &desplazamiento, dtb.quantum);
	concatenarChar(buffer, &desplazamiento, dtb.estado);
	enviarMensaje(receptor, buffer, tamanioBuffer);

	free(buffer);
}

DTB* deserializarDTB(int emisor){
	DTB* dtb = asignarMemoria(sizeof(DTB));
	dtb->escriptorio = deserializarString(emisor);
	dtb->flag = deserializarInt(emisor);
	dtb->id = deserializarInt(emisor);
	dtb->programCounter = deserializarInt(emisor);
	dtb->listaDeArchivos = deserializarListaString(emisor);
	dtb->quantum = deserializarInt(emisor);
	dtb->estado = deserializarChar(emisor);
	return dtb;
}

void freeDTB(DTB* dtb){
	void destruirLista(t_list* lista){
		if(lista->elements_count != 0)
		list_destroy(lista);
	}
	free(dtb->escriptorio);
	list_destroy_and_destroy_elements(dtb->listaDeArchivos, free);
	free(dtb);
}
void freeDTBSAFA(DTB* dtb){
	void destruirLista(t_list* lista){
		if(lista->elements_count != 0)
		list_destroy(lista);
	}
	list_destroy_and_destroy_elements(dtb->listaDeArchivos, free);
	free(dtb);
}

