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

char deserializarIdentificarse(u_int32_t emisor){
	char modulo;
	recibirMensaje(emisor, &modulo, sizeof(char));
	printf("Se identifico a %c \n" , modulo);
	return modulo;
}

void enviarYSerializarString(u_int32_t destino, char* texto,char operacion){
	u_int32_t tamanioTexto = strlen(texto) + 1;
	u_int32_t tamanioMensaje = sizeof(char) + sizeof(u_int32_t) + tamanioTexto;
	void* mensaje = asignarMemoria(tamanioMensaje);

	u_int32_t desplazamiento = 0;

	concatenarChar(mensaje, &desplazamiento, operacion);
	concatenarInt(mensaje, &desplazamiento, tamanioTexto);
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

void concatenarChar(void* buffer, u_int32_t* desplazamiento, char mensaje){
	memcpy(buffer + *desplazamiento, &mensaje, sizeof(char));
	*desplazamiento = *desplazamiento + sizeof(char);
}
int concatenarInt(void* buffer, u_int32_t* desplazamiento, u_int32_t numero){
	memcpy(buffer + *desplazamiento, &numero, sizeof(u_int32_t));
	*desplazamiento = *desplazamiento + sizeof(u_int32_t);
}
void concatenarString(void* buffer, u_int32_t* desplazamiento, char* mensaje){
	memcpy(buffer + *desplazamiento, mensaje, strlen(mensaje) + 1);
	*desplazamiento = *desplazamiento + strlen(mensaje) + 1;
}

char* deserializarString(u_int32_t emisor){
	u_int32_t tamanioMensaje = deserializarInt(emisor);
	char* mensaje = malloc(tamanioMensaje);
	recibirMensaje(emisor, mensaje, tamanioMensaje);
	printf("Recibi %s de parte de %d \n" , mensaje, emisor);
	return mensaje;
}

int deserializarInt(u_int32_t emisor){
	u_int32_t mensaje;
	recibirMensaje(emisor, &mensaje, sizeof(u_int32_t));
	return mensaje;
}

/*void serializarMensajito(mensajito mensaje){
	void* m = asignarMemoria(sizeof(mensaje));
	char operacion = MANDAR_MENSAJITO;
}*/
