#ifndef SEGMENTACIONPURA_H_
#define SEGMENTACIONPURA_H_

#include "FM9.h"

typedef struct{
	int id;
	int limite;
	int base;
}ElementoTablaSegPura;

t_list* tablaDeSegmentos; //Es una lista de ElementoTablaSegPura
respuestaDeCargaEnMemoria guardarDatosSegPura(char* datos);
respuestaDeObtencionDeMemoria* obtenerDatosSegPura(t_list* posiciones);

#endif /* SEGMENTACIONPURA_H_ */
