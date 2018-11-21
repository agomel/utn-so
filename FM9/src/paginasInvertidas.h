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
pthread_mutex_t mutexPaginasInvertidas;
pthread_mutex_t mutexArchivos;

void inicializarInvertida(t_config* configuracion);
RespuestaGuardado* guardarDatosInvertida(int idDTB, char* datos, char* nombreArchivo);
respuestaDeObtencionDeMemoria* obtenerDatosInvertida(int idDTB, char* nombreArchivo);
respuestaDeObtencionDeMemoria* obtenerLineaInvertida(int idDTB, char* nombreArchivo, int numeroLinea);
int asignarDatosInvertida(int IdDTB, char* nombreArchivo, int numeroLinea, char* datos);
void liberarMemoriaInvertida(int idDTB, char* nombreArchivo);
void liberarDTBDeMemoriaInvertida(int idDTB);
void dumpInvertida(int idDTB);


#endif /* PAGINASINVERTIDAS_H_ */
