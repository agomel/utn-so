#ifndef SERVICIOFM9_H_
#define SERVICIOFM9_H_

#include <biblioteca/utilidades.h>
#include <biblioteca/serializacion.h>

extern int socketFM9;

int enviarDatosAFM9(char* datos);
int pedirDatosAFM9(t_list* direcciones);
char* recibirFlushFM9(int transferSize);
t_list* recibirListaDeDireccionesDeFM9();

#endif /*SERVICIOFM9_H_*/
