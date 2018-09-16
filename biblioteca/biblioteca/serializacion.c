#include "serializacion.h"

void handshake(u_int32_t servidor, char modulo){
	u_int32_t tamanioMensaje = sizeof(char)*2;
	void* mensaje = asignarMemoria(tamanioMensaje);
	char operacion = IDENTIFICARSE;
	u_int32_t desplazamiento = 0;
	memcpy(mensaje + desplazamiento, &operacion, sizeof(char));
	desplazamiento = desplazamiento + sizeof(char);
	memcpy(mensaje + desplazamiento, &modulo, sizeof(char));
	desplazamiento = desplazamiento + sizeof(char); //este paso no es necesario ya que no vuelvo a usar el desplazamiento
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

	char operacion = MANDAR_TEXTO;
	u_int32_t desplazamiento = 0;

	concatenarChar(mensaje, &desplazamiento, operacion);
	concatenarInt(mensaje, &desplazamiento, tamanioTexto);

	memcpy(mensaje + desplazamiento, texto, tamanioTexto);
	desplazamiento = desplazamiento + tamanioTexto; //esta linea no es necesaria ya que no vuelvo a utilizar el puntero

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


