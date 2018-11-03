#ifndef SERVICIOFM9_H_
#define SERVICIOFM9_H_

#include <biblioteca/utilidades.h>
#include <biblioteca/serializacion.h>

extern int socketFM9;

extern int transferSize;

int enviarDatosAFM9(int idDTB, char* path, char* datos, char header);
void pedirDatosAFM9(int idDTB, char* path);
char* recibirFlushFM9(int cantidadDeLineas);
t_list* recibirListaDeDireccionesDeFM9();

#endif /*SERVICIOFM9_H_*/
