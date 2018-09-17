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
	dtb.flag = 0;
	dtb.id = obtenerId();
	//TODO poner valor valido en programCounter
	dtb.programCounter = -1;
	//TODO poner valor valido en tablaDireccionesArchivos
	dtb.tablaDireccionesArchivos = -1;

	return dtb;
}

void serializarYEnviarDTB(int receptor, DTB dtb, char operacion){
	//Asigno tamanio al buffer
	u_int32_t tamanioEscriptorio = strlen(dtb.escriptorio) + 1;
	u_int32_t tamanioBuffer = sizeof(u_int32_t)*4 + tamanioEscriptorio;
	void* buffer = asignarMemoria(tamanioBuffer);

	//Lleno el buffer
	u_int32_t desplazamiento = 0;

	concatenarChar(buffer, &desplazamiento, operacion);
	concatenarString(buffer, &desplazamiento, dtb.escriptorio);
	concatenarInt(buffer, &desplazamiento, dtb.flag);
	concatenarInt(buffer, &desplazamiento, dtb.id);
	concatenarInt(buffer, &desplazamiento, dtb.programCounter);
	concatenarInt(buffer, &desplazamiento, dtb.tablaDireccionesArchivos);

	enviarMensaje(receptor, buffer, tamanioBuffer);

	free(buffer);
}

DTB deserializarDTB(int emisor){
	DTB dtb;
	dtb.escriptorio = deserializarString(emisor);
	dtb.flag = deserializarInt(emisor);
	dtb.id = deserializarInt(emisor);
	dtb.programCounter = deserializarInt(emisor);
	dtb.tablaDireccionesArchivos = deserializarInt(emisor);

	return dtb;
}
