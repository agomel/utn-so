#ifndef SERIALIZACION_H_
#define SERIALIZACION_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <commons/collections/dictionary.h>
#include "utilidades.h"
#include "dtb.h"

typedef struct{
		void* mensaje;
		u_int32_t tamanioMensaje;
	}voidDeserealizado;


void handshake(u_int32_t servidor, char modulo);

void enviarStringSerializado(u_int32_t destino, char* texto,char operacion);
void enviarYSerializarInt(u_int32_t destino, u_int32_t numero,char operacion);

char* deserializarString(u_int32_t emisor);
int deserializarInt(u_int32_t emisor);
char deserializarChar(u_int32_t emisor);
void concatenarChar(void* buffer, u_int32_t* desplazamiento, char mensaje);
int concatenarInt(void* buffer, u_int32_t* desplazamiento, u_int32_t numero);
void concatenarString(void* buffer, u_int32_t* desplazamiento, char* mensaje);

voidDeserealizado deserializarVoid(u_int32_t emisor);

#endif /*SERIALIZACION_H_*/
