#ifndef SEGMENTACIONPAG_H_
#define SEGMENTACIONPAG_H_

#include "FM9.h"

typedef struct{
	int id;
	t_list* paginas;
	int cantidadLineas;
	char* nombreArchivo;
}ElementoTablaSegPag;

typedef struct{
	int idPag;
	int marco;
}ElementoTablaPag;

typedef struct{
	int idDTB;
	t_list* segmentos;
}ElementoTablaDTBS;

typedef struct{
	int resultado;
	ElementoTablaSegPag* elementoTabla;
	int pesoArchivo;
}RespuestaCargaSegPag;

int idSegmento;
int idPagina;
t_list* tablaDePaginas;
t_list* tablaDeProcesos;

void inicializarSegPag();
RespuestaGuardado* guardarDatosSegPag(int idDTB, char* datos, char* nombreArchivo);
RespuestaGuardado* nuevoProcesoSegPag(int idDTB, char* datos, char* nombreArchivo);
respuestaDeObtencionDeMemoria* obtenerDatosSegPag(int idDTB, char* idsSegmentos);
respuestaDeObtencionDeMemoria* obtenerLineaSegPag(int idDTB, char* nombreArchivo, int numeroLinea);
void liberarMemoriaSegPag(int idDTB, char* nombreArchivo);
void liberarDTBDeMemoriaSegPag(int idDTB);
ElementoTablaPag* obtenerPaginasPorId(int pagina);

#endif /* SEGMENTACIONPAG_H_ */
