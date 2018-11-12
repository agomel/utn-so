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
	int resultado;
	ElementoTablaSegPura* elementoTabla;
	int pesoArchivo;
}RespuestaCargaSegPura;

int idSegmento;
t_list* tablaDeProcesos; //Es una lista de ElementoTablaProcesos
t_list* segmentosOcupados; //Es una lista de SegmentoOcupado

void inicializarSegPura();
RespuestaGuardado* guardarDatosSegPura(int idDTB, char* datos, char* nombreArchivo);
RespuestaGuardado* nuevoProcesoSegPura(int idDTB, char* datos, char* nombreArchivo);
respuestaDeObtencionDeMemoria* obtenerDatosSegPura(int idDTB, char* nombreArchivo);
respuestaDeObtencionDeMemoria* obtenerLineaSegPura(int idDTB, char* nombreArchivo, int numeroLinea);
void liberarMemoriaSegPura(int idDTB, char* nombreArchivo);
int asignarDatosSegPura(int IdDTB, char* nombreArchivo, int numeroLinea, char* datos);

#endif /* SEGMENTACIONPURA_H_ */
