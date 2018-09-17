#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <biblioteca/dtb.h>
#include <commons/collections/queue.h>

t_queue* colaNEW;
t_queue* colaREADY;
t_queue* colaEXECUTE;
t_queue* colaBLOCKED;

void inicializarColas();



#endif /*PLANIFICADOR_H_*/
