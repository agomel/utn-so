#ifndef PAGINASINVERTIDAS_H_
#define PAGINASINVERTIDAS_H_

#include "FM9.h"

typedef struct{
	int marco;
	int pagina;
	int idDTB;
	char* nombreArchivo;
}ElementoTablaInvertida;

typedef struct{
	int resultado;
	int pesoArchivo;
}RespuestaCargaInvertida;

typedef struct{
	int idDTB;
	int cantidadLineas;
	char* nombreArchivo;
}ElementoArchivos;


t_list* tablaPaginasInvertidas;
t_list* tablaDeArchivos;
int idPagina;
int idMarco;

void inicializarInvertida(t_config* configuracion);
RespuestaGuardado* guardarDatosInvertida(int idDTB, char* datos, char* nombreArchivo);
respuestaDeObtencionDeMemoria* obtenerDatosInvertida(int idDTB, char* nombreArchivo);
respuestaDeObtencionDeMemoria* obtenerLineaInvertida(int idDTB, char* nombreArchivo, int numeroLinea);



#endif /* PAGINASINVERTIDAS_H_ */
