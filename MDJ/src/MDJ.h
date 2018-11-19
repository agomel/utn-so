#ifndef MDJ_H_
#define MDJ_H_

#include <stdio.h>
#include <stdlib.h>
#include <biblioteca/utilidades.h>
#include <biblioteca/socket.h>
#include <biblioteca/select.h>
#include <biblioteca/hilos.h>
#include <commons/collections/queue.h>
#include <biblioteca/semaforos.h>
#include <biblioteca/nuestroSelect.h>
#include <biblioteca/traductor.h>
#include "consola.h"
#include "bitmap.h"
#include "fileSystem.h"
#include "operacionesFIFA.h"

int RETARDO;
char* PUNTO_MONTAJE;
char* MONTAJE_ACTUAL;

char* PUNTO_MONTAJE_ARCHIVOS;
char* PUNTO_MONTAJE_METADATA;
char* PUNTO_MONTAJE_BLOQUES;
int TAMANIO_BLOQUES;
int CANTIDAD_BLOQUES;
char* MAGIC_NUMBER;

t_queue* colaOperaciones;
pthread_mutex_t mutexOperaciones;
sem_t semOperaciones;
sem_t semProductores;

t_log* logger;
t_config* configuracion;

void entenderMensaje(int emisor, char header);

int identificarse(int emisor, char header);
void crearSelect(int servidor);
void levantarMetadata();

void obtenerPuntoMontaje(char* primerMontaje);
void crearPuntosDeMontaje();
void init();
#endif /*MDJ_H_*/
