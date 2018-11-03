#ifndef SEGMENTACIONPAG_H_
#define SEGMENTACIONPAG_H_

#include "FM9.h"

typedef struct{
	int id;
	t_list* paginas;
	int cantidadLineas;
	char* nombreArchivo;
}ElementoTablaSeg;

typedef struct{
	int idPag;
	int marco;
}ElementoTablaPag;

int idSegmento;
int idPagina;
t_list* tablaDeSegmentos;
t_list* tablaDePaginas;

void inicializarSegPag();
int guardarDatosSegPag(int idDTB, char* datos, char* nombreArchivo);
respuestaDeObtencionDeMemoria* obtenerDatosSegPag(char* idsSegmentos);
respuestaDeObtencionDeMemoria* obtenerLineaSegPag(char* nombreArchivo, int numeroLinea);
void liberarMemoriaSegPag(char* nombreArchivo);
ElementoTablaPag* obtenerPaginasPorId(int pagina);

#endif /* SEGMENTACIONPAG_H_ */
