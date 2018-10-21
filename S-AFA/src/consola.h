#ifndef CONSOLA_H_
#define CONSOLA_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "S-AFA.h"
#include "gestorDeDTBs.h"
#include <biblioteca/dtb.h>
#include <biblioteca/traductor.h>

enum{
	SALIR = 0, STATUS = 1, FINALIZAR = 2, METRICAS = 3, EJECUTAR=4
};

typedef struct{
	char* comando;
	char* parametro;
}comandoCompleto;

int obtenerComando(char*ingresado);
comandoCompleto rearmarCadena(char* cadenaIngresada);
void consola();

#endif /*CONSOLA_H_*/
