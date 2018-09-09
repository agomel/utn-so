#ifndef CONSOLA_H_
#define CONSOLA_H_


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
enum{
	salir=0,status=1,finalizar=2,metricas=3,ejecutar=4
};

typedef struct{
	char* comando;
	char* parametro;
}comandoCompleto;



int obtenerComando(char*ingresado);
comandoCompleto rearmarCadena(char* cadenaIngresada);

void consola();

#endif /*CONSOLA_H_*/
