#ifndef PAGINASINVERTIDAS_H_
#define PAGINASINVERTIDAS_H_

#include "FM9.h"

typedef struct{
	int marco;
	int pagina;
	int idDTB;
	char* nombreArchivo;
}ElementoTablaInvertida;

t_list* tablaPaginasInvertidas;
int idPagina;
int idMarco;

void inicializarInvertida(t_config* configuracion);
int guardarDatosInvertida(int idDTB, char* datos, char* nombreArchivo);
respuestaDeObtencionDeMemoria* obtenerDatosInvertida(char* ids);


#endif /* PAGINASINVERTIDAS_H_ */
