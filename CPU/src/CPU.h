#ifndef CPU_H_
#define CPU_H_

#include <biblioteca/socket.h>
#include <biblioteca/hilos.h>
#include <biblioteca/utilidades.h>
#include <biblioteca/dtb.h>
#include <biblioteca/logger.h>
#include <unistd.h>
#include "operaciones.h"
#include "parser.h"
#include "comunicaciones.h"

int socketDIEGO;
int socketFM9;
int socketSAFA;
t_log* logger;
int retardo;
int sentencias;

#endif /*CPU_H_*/
