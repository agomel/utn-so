#ifndef SERVICIOSAFA_H_
#define SERVICIOSAFA_H_

#include <biblioteca/utilidades.h>
#include <biblioteca/serializacion.h>
#include <biblioteca/logger.h>

extern int socketSAFA;

extern t_log* logger;

void notificarASafaExitoDeCarga(int idDTB, char* path, t_list* direcciones);
void notificarASafaExitoDeGuardado(int idDTB, char* path);
void enviarError(int idDTB, char* path, int error);

#endif /*SERVICIOSAFA_H_*/
