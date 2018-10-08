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
#include <commons/collections/list.h>
#include "utilidades.h"
#include "dtb.h"

typedef struct{
		void* mensaje;
		int tamanioMensaje;
	}voidDeserealizado;


void handshake(int servidor, char modulo);

void enviarYSerializarString(int destino, char* texto,char operacion);
void enviarYSerializarStringSinHeader(int destino, char* texto);
void enviarYSerializarInt(int destino, int numero,char operacion);
void enviarYSerializarIntSinHeader(int destino, int numero);
char* deserializarString(int emisor);
int deserializarInt(int emisor);
char deserializarChar(int emisor);
t_list* deserializarListaInt(int emisor);
t_dictionary* deserializarDiccionario(int emisor);
int obtenerTamanioDiccionario(t_dictionary* diccionario);
void concatenarChar(void* buffer, int* desplazamiento, char mensaje);
void concatenarInt(void* buffer, int* desplazamiento, int numero);
void concatenarString(void* buffer, int* desplazamiento, char* mensaje);
void concatenarListaInt(void* buffer, int* desplazamiento, t_list* listaArchivos);
void concatenarDiccionario(void* buffer, int* desplazamiento, t_dictionary* diccionario);

voidDeserealizado deserializarVoid(int emisor);

#endif /*SERIALIZACION_H_*/
