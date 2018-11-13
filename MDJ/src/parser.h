#ifndef PARSER_H_
#define PARSER_H_

#include "operacionesArchivos.h"


typedef struct{
	char** bloques;
	int tamanio;
}Metadata;

Metadata obtenerMetadata(char* path);
#endif /*PARSER_H_*/
