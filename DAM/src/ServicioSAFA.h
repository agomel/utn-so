#ifndef SERVICIOSAFA_H_
#define SERVICIOSAFA_H_

#include <biblioteca/utilidades.h>
#include <biblioteca/serializacion.h>
#include <biblioteca/logger.h>

extern int socketSAFA;

extern t_log* logger;

void notificarASafaExito(char header ,int idDTB, char* path, int pesoArchivo);
void enviarError(int idDTB, char* path, int error);

#endif /*SERVICIOSAFA_H_*/
