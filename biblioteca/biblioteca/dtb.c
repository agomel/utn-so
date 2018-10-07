#include "dtb.h"

u_int32_t obtenerId(){
	waitMutex(&mutexIdsDTB);
	u_int32_t id = contadorIds;
	contadorIds++;
	signalMutex(&mutexIdsDTB);
	return id;
}

DTB crearDTB (char* parametro){
	DTB dtb;
	dtb.escriptorio = parametro;
	dtb.flag = 1;
	dtb.id = obtenerId();
	printf("id del dtb %d \n",dtb.id);
	dtb.tablaDireccionesArchivos = list_create();
	//TODO poner valor valido en programCounter
	dtb.programCounter = 0;
	dtb.estado = NEW;
	return dtb;
}

void serializarYEnviarDTB(int receptor, DTB dtb){
	//Asigno tamanio al buffer
	u_int32_t tamanioEscriptorio = strlen(dtb.escriptorio) + 1;
	u_int32_t tamanioLista = dtb.tablaDireccionesArchivos->elements_count;
	u_int32_t tamanioBuffer = sizeof(char)*2 + sizeof(u_int32_t)*6 + tamanioEscriptorio + sizeof(u_int32_t)*(tamanioLista);
	void* buffer = asignarMemoria(tamanioBuffer);

	//Lleno el buffer
	u_int32_t desplazamiento = 0;

	concatenarChar(buffer, &desplazamiento, ENVIAR_DTB);
	concatenarString(buffer, &desplazamiento, dtb.escriptorio);
	concatenarInt(buffer, &desplazamiento, dtb.flag);
	concatenarInt(buffer, &desplazamiento, dtb.id);
	printf("Concatenando id %d \n",dtb.id);
	concatenarInt(buffer, &desplazamiento, dtb.programCounter);
	concatenarListaInt(buffer, &desplazamiento, dtb.tablaDireccionesArchivos);
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
	printf("Deserializando id %d \n",dtb.id);
	dtb.programCounter = deserializarInt(emisor);
	dtb.tablaDireccionesArchivos = deserializarListaInt(emisor);
	dtb.quantum = deserializarInt(emisor);
	dtb.estado = deserializarChar(emisor);
	return dtb;
}


DTB* obtenerDTBDeColaRemoviendolo(t_list* cola, u_int32_t idDTB){
	DTB* dtb;
	int index = 0;
	for(int index = 0; index < cola->elements_count; index++){
		dtb = list_get(cola, index);
		if(dtb->id == idDTB){
			list_remove(cola, index);
			break;
		}
	}
	return dtb;
}

DTB* obtenerDTBDeCola(t_list* cola, u_int32_t idDTB){
	DTB* dtb;
	int index = 0;
	for(int index = 0; index < cola->elements_count; index++){
		dtb = list_get(cola, index);
		if(dtb->id == idDTB && dtb->flag != 0){
			break;
		}
	}
	return dtb;
}

void freeDTB(DTB* dtb){
	free(dtb->escriptorio);
	free(dtb->estado);
	free(dtb->flag);
	free(dtb->id);
	free(dtb->programCounter);
	free(dtb->quantum);
	free(dtb);
}

