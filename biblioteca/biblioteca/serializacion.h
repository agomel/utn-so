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


void deserializar(char** parametros,int emisor);
void deserializarIdentificarse(int emisor,t_dictionary* conexiones);

char* serializarMensaje(int operacion,char* mensaje);

#endif /*SERIALIZACION_H_*/
