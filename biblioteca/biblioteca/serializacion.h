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

void handshake(u_int32_t servidor, char modulo);

char deserializarIdentificarse(u_int32_t emisor);

void enviarStringSerializado(u_int32_t destino, char* texto);

void deserializarString(u_int32_t emisor);

void concatenarChar(void* buffer, u_int32_t* desplazamiento, char mensaje);
int concatenarInt(void* buffer, u_int32_t* desplazamiento, u_int32_t numero);

#endif /*SERIALIZACION_H_*/
