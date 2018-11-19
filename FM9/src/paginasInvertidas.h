#ifndef PAGINASINVERTIDAS_H_
#define PAGINASINVERTIDAS_H_

#include "FM9.h"

typedef struct{
	int marco;
	int pagina;
	int idDTB;
	char* nombreArchivo;
}ElementoTablaInvertida;



int guardarDatosInvertida(int idDTB, char* datos, char* nombreArchivo);
respuestaDeObtencionDeMemoria* obtenerDatosInvertida(char* ids);


#endif /* PAGINASINVERTIDAS_H_ */
