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
	dtb.direccionesArchivos = dictionary_create();
	//TODO poner valor valido en programCounter
	dtb.programCounter = 0;
	dtb.estado = NEW;
	return dtb;
}

void serializarYEnviarDTB(int receptor, DTB dtb, t_log* logger){
	//Asigno tamanio al buffer
	log_debug(logger, "Enviando dtb con id: %d", dtb.id);
	int tamanioEscriptorio = strlen(dtb.escriptorio) + 1;
	int tamanioBuffer = sizeof(char)*2 + sizeof(int)*6 + tamanioEscriptorio + obtenerTamanioDiccionario(dtb.direccionesArchivos);
	void* buffer = asignarMemoria(tamanioBuffer);

	//Lleno el buffer
	int desplazamiento = 0;

	concatenarChar(buffer, &desplazamiento, ENVIAR_DTB);
	concatenarString(buffer, &desplazamiento, dtb.escriptorio);
	concatenarInt(buffer, &desplazamiento, dtb.flag);
	concatenarInt(buffer, &desplazamiento, dtb.id);
	concatenarInt(buffer, &desplazamiento, dtb.programCounter);
	concatenarDiccionario(buffer, &desplazamiento, dtb.direccionesArchivos);
	concatenarInt(buffer, &desplazamiento, dtb.quantum);
	concatenarChar(buffer, &desplazamiento, dtb.estado);
	enviarMensaje(receptor, buffer, tamanioBuffer);

	free(buffer);
}

DTB deserializarDTB(int emisor){
	DTB dtb;
	dtb.escriptorio = deserializarString(emisor);;
	dtb.flag = deserializarInt(emisor);
	dtb.id = deserializarInt(emisor);
	dtb.programCounter = deserializarInt(emisor);
	dtb.direccionesArchivos = deserializarDiccionario(emisor);
	dtb.quantum = deserializarInt(emisor);
	dtb.estado = deserializarChar(emisor);
	return dtb;
}

void freeDTB(DTB* dtb){
	free(dtb->escriptorio);
	free(dtb->estado);
	free(dtb->flag);
	free(dtb->id);
	free(dtb->programCounter);
	free(dtb->quantum);
	//TODO falta frear las listas
	free(dtb);
}

