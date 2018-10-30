#ifndef SERVICIOFM9_H_
#define SERVICIOFM9_H_

#include <biblioteca/utilidades.h>
#include <biblioteca/serializacion.h>
#include <biblioteca/logger.h>

extern int socketFM9;

extern t_log* logger;

extern int transferSize;

int enviarDatosAFM9(char* path, char* datos);
void pedirDatosAFM9(char* path);
char* recibirFlushFM9();
t_list* recibirListaDeDireccionesDeFM9();

#endif /*SERVICIOFM9_H_*/
