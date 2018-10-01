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

typedef struct{
	u_int32_t id_dtb;
	t_list* listaDeDirecciones;
	u_int32_t pudoGuardarlo;
}respuestaDeCargaEnMemoria;


respuestaDeCargaEnMemoria cargarDatosEnMemoria(char* datos, u_int32_t id_dtb);

void entenderMensaje(int emisor, int header);
int main(void);

#endif /*FM9_H_*/
