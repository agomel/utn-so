#ifndef DAM_H_
#define DAM_H_

#include <biblioteca/socket.h>
#include <biblioteca/hilos.h>
#include <biblioteca/utilidades.h>
#include <biblioteca/select.h>
#include <biblioteca/serializacion.h>
#include <biblioteca/dtb.h>
#include <commons/collections/queue.h>

u_int32_t socketFM9;
u_int32_t socketMDJ;
u_int32_t socketSAFA;
t_queue* colaOperaciones;

pthread_mutex_t mutexColaOperaciones;
sem_t semHayEnColaOperaciones;

typedef struct{
	char* path;
	u_int32_t idDTB;
	u_int32_t offset;
	u_int32_t size;
	char accion;
}Operacion;

void entenderMensaje(int emisor, char header);

void escuchar(int servidor);

int main(void);

void inicializarDAM();

void agregarDTBAColaMDJ(int emisor, int esDummy, char operacion);

void enviarAMDJ();

#endif /*DAM_H_*/
