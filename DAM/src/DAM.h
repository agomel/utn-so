#ifndef DAM_H_
#define DAM_H_

#include <biblioteca/socket.h>
#include <biblioteca/hilos.h>
#include <biblioteca/utilidades.h>
#include <biblioteca/select.h>
#include <biblioteca/serializacion.h>
#include <biblioteca/dtb.h>
#include <commons/collections/queue.h>
#include <biblioteca/logger.h>
#include <biblioteca/nuestroSelect.h>
#include "ServicioFM9.h"
#include <biblioteca/traductor.h>

int transferSize;

int socketFM9;
int socketMDJ;
int socketSAFA;

t_queue* colaOperaciones;

pthread_mutex_t mutexColaOperaciones;
sem_t semHayEnColaOperaciones;
sem_t semProductores;

t_config* configuracion;

t_log* logger;


void inicializarDAM();
int main(void);


#endif /*DAM_H_*/
