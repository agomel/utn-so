#ifndef S-AFA_H_
#define S-AFA_H_

#include <biblioteca/socket.h>
#include <biblioteca/hilos.h>
#include <biblioteca/utilidades.h>
#include "consola.h"
#include <biblioteca/select.h>
#include "plp.h"
#include "pcp.h"
#include <biblioteca/dtb.h>
#include <biblioteca/logger.h>
#include <biblioteca/nuestroSelect.h>
#include <biblioteca/traductor.h>

int socketDAM;
pthread_mutex_t mutexDummy;
int socketDAM;
int conectadoCPU;
int conectadoDAM;
t_list* socketsCPUs;
pthread_mutex_t mutexSocketsCPus;
pthread_mutex_t mutexEjecutandoCPU;
//Clave idDTB valores sockets CPU
t_dictionary* ejecutandoCPU;
pthread_mutex_t mutexCpusAFinalizarDTBs;
//Clave sockets CPU valores idDTB
t_dictionary* cpusAFinalizarDTBs;
t_dictionary* recursos;
pthread_mutex_t mutexRecursos;


t_list* esperandoRecursos;
pthread_mutex_t mutexEsperandoRecursos;

enum ESTADOSAFA{
	CORRUPTO = 'c', OPERATIVO = 'a'
};

typedef struct{
	int socket;
	int ocupado;
}SocketCPU;

typedef struct{
	int idDTB;
	char* recurso;
}DTBEsperandoRecurso;


char estado;
t_log* logger;
t_queue* colaOperaciones;
pthread_mutex_t mutexOperaciones;
sem_t semOperaciones;
sem_t semProductores;

int retardo;
void entenderMensaje(int emisor, char header);

int main(void);

#endif /*S-AFA_H_*/
