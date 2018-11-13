#ifndef PARSER_H_
#define PARSER_H_

#include "operacionesArchivos.h"

extern char* TAMANIO_BLOQUES;

typedef struct{
	char** bloques;
	int tamanio;
}Metadata;

Metadata* obtenerMetadata(char* path);
void freeMetadata(Metadata* metadata);
#endif /*PARSER_H_*/
