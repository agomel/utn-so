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
	dtb.tablaDireccionesArchivos = list_create();
	//TODO poner valor valido en programCounter
	dtb.programCounter = 0;
	return dtb;
}

void serializarYEnviarDTB(int receptor, DTB dtb){
	//Asigno tamanio al buffer
	u_int32_t tamanioEscriptorio = strlen(dtb.escriptorio) + 1;
	u_int32_t tamanioLista = dtb.tablaDireccionesArchivos->elements_count;
	u_int32_t tamanioBuffer = sizeof(char) + sizeof(u_int32_t)*5 + tamanioEscriptorio + sizeof(u_int32_t)*(tamanioLista);
	void* buffer = asignarMemoria(tamanioBuffer);

	//Lleno el buffer
	u_int32_t desplazamiento = 0;

	concatenarChar(buffer, &desplazamiento, ENVIAR_DTB);
	concatenarString(buffer, &desplazamiento, dtb.escriptorio);
	concatenarInt(buffer, &desplazamiento, dtb.flag);
	concatenarInt(buffer, &desplazamiento, dtb.id);
	concatenarInt(buffer, &desplazamiento, dtb.programCounter);
	concatenarListaInt(buffer, &desplazamiento, dtb.tablaDireccionesArchivos);

	enviarMensaje(receptor, buffer, tamanioBuffer);

	free(buffer);
}

DTB deserializarDTB(int emisor){
	DTB dtb;
	dtb.escriptorio = deserializarString(emisor);
	dtb.flag = deserializarInt(emisor);
	dtb.id = deserializarInt(emisor);
	dtb.programCounter = deserializarInt(emisor);
	dtb.tablaDireccionesArchivos = deserializarListaInt(emisor);

	return dtb;
}


DTB* obtenerProcesoDeCola(t_list* cola, u_int32_t idDTB){
	DTB* dtb;
	int index = 0;
	int salir = 0;
	while(index < cola->elements_count && !salir){
		dtb = list_get(cola, index);
		if(dtb->id == idDTB){
			salir = 1;
			list_remove(cola, index);
		}
		index++;
	}
	return dtb;
}
