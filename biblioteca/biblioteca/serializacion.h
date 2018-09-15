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


//void deserializar(void** parametros,int emisor);
//void deserializarIdentificarse(int emisor,t_dictionary* conexiones);

//char* serializarMensaje(int operacion,char* mensaje);

void handshake(u_int32_t servidor, char modulo);

void deserializarIdentificarse(u_int32_t emisor);

void enviarStringSerializado(u_int32_t destino, char* texto);

void deserializarString(u_int32_t emisor);

#endif /*SERIALIZACION_H_*/
