#ifndef SEGMENTACIONPURA_H_
#define SEGMENTACIONPURA_H_

#include "FM9.h"

typedef struct{
	int id;
	int limite;
	int base;
	char* nombreArchivo;
}ElementoTablaSegPura;

typedef struct{
	int idDTB;
	t_list* tablaSegmentos;
}ElementoTablaProcesos;

typedef struct{
	int base;
	int limite;
}SegmentoOcupado;

typedef struct{
	int respuestaGuardado;
	ElementoTablaSegPura* elementoTabla;
}RespuestaCargaSegPura;

int idSegmento;
t_list* tablaDeProcesos; //Es una lista de ElementoTablaProcesos
t_list* segmentosOcupados; //Es una lista de SegmentoOcupado

void inicializarSegPura();
int guardarDatosSegPura(int idDTB, char* datos, char* nombreArchivo);
int nuevoProcesoSegPura(int idDTB, char* datos, char* nombreArchivo);
respuestaDeObtencionDeMemoria* obtenerDatosSegPura(char* nombreArchivo);
respuestaDeObtencionDeMemoria* obtenerLineaSegPura(char* nombreArchivo, int numeroLinea);
void liberarMemoriaSegPura(int idDTB, char* nombreArchivo);

#endif /* SEGMENTACIONPURA_H_ */
