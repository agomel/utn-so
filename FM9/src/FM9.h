#ifndef FM9_H_
#define FM9_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <biblioteca/utilidades.h>
#include <biblioteca/socket.h>
#include <biblioteca/select.h>
#include <biblioteca/hilos.h>
#include <commons/collections/list.h>

t_dictionary* conexiones;
int socketCPU;
int socketDAM;
char* storage;
pthread_mutex_t mutexStorage;
int offset;
pthread_mutex_t mutexOffset;


typedef struct{
	t_list* listaDeDirecciones;
	int pudoGuardarlo;
}respuestaDeCargaEnMemoria;

typedef struct{
	t_list* listaDeDirecciones;
	int cantidadDeLineas;
	int pudoGuardarlo;
}respuestaDeObtencionDeMemoria;


respuestaDeCargaEnMemoria cargarDatosEnMemoria(char* datos);

void init();
void entenderMensaje(int emisor, int header);
int main(void);

#endif /*FM9_H_*/
