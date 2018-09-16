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

void enviarStringSerializado(u_int32_t destino, char* texto){
	u_int32_t tamanioTexto = strlen(texto) + 1;
	u_int32_t tamanioMensaje = sizeof(char) + sizeof(u_int32_t) + tamanioTexto;
	void* mensaje = asignarMemoria(tamanioMensaje);

	u_int32_t desplazamiento = 0;

	concatenarChar(mensaje, &desplazamiento, MANDAR_TEXTO);
	concatenarInt(mensaje, &desplazamiento, tamanioTexto);
	concatenarString(mensaje, &desplazamiento, texto);

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

void deserializarString(u_int32_t emisor){
	u_int32_t tamanioMensaje;
	recibirMensaje(emisor, &tamanioMensaje, sizeof(u_int32_t));
	char* mensaje = malloc(tamanioMensaje);
	recibirMensaje(emisor, mensaje, tamanioMensaje);
	printf("Recibi %s de parte de %d \n" , mensaje, emisor);
}

/*void serializarMensajito(mensajito mensaje){
	void* m = asignarMemoria(sizeof(mensaje));
	char operacion = MANDAR_MENSAJITO;



}*/


