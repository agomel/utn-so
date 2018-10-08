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

int socketCPU;
int socketDAM;
pthread_mutex_t mutexDummy;
int socketCPU;
int socketDAM;
int conectadoCPU;
int conectadoDAM;

enum ESTADOSAFA{
	CORRUPTO = 'c', OPERATIVO = 'a'
};

char estado;
t_log* logger;

void entenderMensaje(int emisor, char header);

int main(void);

#endif /*S-AFA_H_*/
