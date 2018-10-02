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
u_int32_t socketCPU;
u_int32_t socketDAM;
char* storage;
pthread_mutex_t mutexStorage;
u_int32_t offset;
pthread_mutex_t mutexOffset;
t_config* archivoConfiguracion;


typedef struct{
	t_list* listaDeDirecciones;
	u_int32_t pudoGuardarlo;
}respuestaDeCargaEnMemoria;


respuestaDeCargaEnMemoria cargarDatosEnMemoria(char* datos);

void init();
void entenderMensaje(int emisor, int header);
int main(void);

#endif /*FM9_H_*/
