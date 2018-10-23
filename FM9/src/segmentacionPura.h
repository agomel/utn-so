#ifndef SEGMENTACIONPURA_H_
#define SEGMENTACIONPURA_H_

#include "FM9.h"

typedef struct{
	int id;
	int limite;
	int base;
}ElementoTablaSegPura;

int idSegmento;
t_list* tablaDeSegmentos; //Es una lista de ElementoTablaSegPura

void inicializarSegPura();
respuestaDeCargaEnMemoria guardarDatosSegPura(char* datos);
respuestaDeObtencionDeMemoria* obtenerDatosSegPura(t_list* idsSegmentos);
respuestaDeObtencionDeMemoria* obtenerLineaSegPura(t_list* ids, int numeroLinea);
#endif /* SEGMENTACIONPURA_H_ */
