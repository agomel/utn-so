#include "serializacion.h"

void handshake(u_int32_t servidor, char modulo){
	u_int32_t tamanioMensaje = sizeof(char)*2;
	void* mensaje = asignarMemoria(tamanioMensaje);
	u_int32_t desplazamiento = 0;

	concatenarChar(mensaje, &desplazamiento, IDENTIFICARSE);
	concatenarChar(mensaje, &desplazamiento, modulo);

	enviarMensaje(servidor, mensaje, tamanioMensaje);
	free(mensaje);
}

void enviarYSerializarString(u_int32_t destino, char* texto,char operacion){
	u_int32_t tamanioTexto = strlen(texto) + 1;
	u_int32_t tamanioMensaje = sizeof(char) + sizeof(u_int32_t) + tamanioTexto;
	void* mensaje = asignarMemoria(tamanioMensaje);

	u_int32_t desplazamiento = 0;

	concatenarChar(mensaje, &desplazamiento, operacion);
	concatenarString(mensaje, &desplazamiento, texto);

	enviarMensaje(destino, mensaje, tamanioMensaje);

	free(mensaje);
}

void enviarYSerializarStringSinHeader(u_int32_t destino, char* texto){
	u_int32_t tamanioTexto = strlen(texto) + 1;
	u_int32_t tamanioMensaje = sizeof(u_int32_t) + tamanioTexto;
	void* mensaje = asignarMemoria(tamanioMensaje);

	u_int32_t desplazamiento = 0;

	concatenarString(mensaje, &desplazamiento, texto);

	enviarMensaje(destino, mensaje, tamanioMensaje);

	free(mensaje);
}
void enviarYSerializarInt(u_int32_t destino, u_int32_t numero,char operacion){
	u_int32_t tamanioMensaje = sizeof(u_int32_t) + sizeof(char);
	void* mensaje = asignarMemoria(tamanioMensaje);

	u_int32_t desplazamiento = 0;

	concatenarChar(mensaje, &desplazamiento, operacion);
	concatenarInt(mensaje, &desplazamiento, numero);

	enviarMensaje(destino, mensaje, tamanioMensaje);

	free(mensaje);
}

void enviarYSerializarIntSinHeader(u_int32_t destino, u_int32_t numero){
	u_int32_t tamanioMensaje = sizeof(u_int32_t);
	void* mensaje = asignarMemoria(tamanioMensaje);

	u_int32_t desplazamiento = 0;

	concatenarInt(mensaje, &desplazamiento, numero);

	enviarMensaje(destino, mensaje, tamanioMensaje);

	free(mensaje);
}

void concatenarChar(void* buffer, u_int32_t* desplazamiento, char mensaje){
	memcpy(buffer + *desplazamiento, &mensaje, sizeof(char));
	*desplazamiento = *desplazamiento + sizeof(char);
}

void concatenarInt(void* buffer, u_int32_t* desplazamiento, u_int32_t numero){
	memcpy(buffer + *desplazamiento, &numero, sizeof(u_int32_t));
	*desplazamiento = *desplazamiento + sizeof(u_int32_t);
}

void concatenarString(void* buffer, u_int32_t* desplazamiento, char* mensaje){
	concatenarInt(buffer, desplazamiento, strlen(mensaje) + 1);
	memcpy(buffer + *desplazamiento, mensaje, strlen(mensaje) + 1);
	*desplazamiento = *desplazamiento + strlen(mensaje) + 1;
}

void concatenarListaInt(void* buffer, u_int32_t* desplazamiento, t_list* listaArchivos){
	concatenarInt(buffer, desplazamiento, listaArchivos->elements_count);
	for(int i = 0; i < (listaArchivos->elements_count); i++){
		concatenarInt(buffer, desplazamiento, list_get(listaArchivos, i));
	}
}

char* deserializarString(u_int32_t emisor){
	u_int32_t tamanioMensaje = deserializarInt(emisor);
	char* mensaje = asignarMemoria(tamanioMensaje);
	recibirMensaje(emisor, mensaje, tamanioMensaje);
	char respuesta[tamanioMensaje];
	for(int i = 0; i<tamanioMensaje; i++){
		respuesta[i] = mensaje[i];
	}
	free(mensaje);
	printf("Recibi %s de parte de %d \n" , respuesta, emisor);
	return respuesta;
}

u_int32_t deserializarInt(u_int32_t emisor){
	u_int32_t mensaje;
	recibirMensaje(emisor, &mensaje, sizeof(u_int32_t));
	return mensaje;
}

char deserializarChar(u_int32_t emisor){
	char mensaje;
	recibirMensaje(emisor, &mensaje, sizeof(char));
	printf("Recibi %c de parte de %d \n" , mensaje, emisor);
	return mensaje;
}

t_list* deserializarListaInt(u_int32_t emisor){
	u_int32_t elementosDeLalista = deserializarInt(emisor);
	t_list* respuesta;
	for(int i = 0; i<elementosDeLalista; i++){
		list_add(respuesta, deserializarInt(emisor));
	}
	return respuesta;
}

voidDeserealizado deserializarVoid(u_int32_t emisor){
	voidDeserealizado mensajeADeserealizar;
	mensajeADeserealizar.tamanioMensaje = deserializarInt(emisor);
	char* mensaje = malloc(mensajeADeserealizar.tamanioMensaje);
	recibirMensaje(emisor, mensaje, mensajeADeserealizar.tamanioMensaje);
	mensajeADeserealizar.mensaje = *mensaje;
	free(mensaje);
	return mensajeADeserealizar;
}
