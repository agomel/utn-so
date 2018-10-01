#ifndef DAM_H_
#define DAM_H_

#include <biblioteca/socket.h>
#include <biblioteca/hilos.h>
#include <biblioteca/utilidades.h>
#include <biblioteca/select.h>
#include <biblioteca/serializacion.h>
#include <biblioteca/dtb.h>

u_int32_t socketCPU;
u_int32_t socketFM9;
u_int32_t socketMDJ;
u_int32_t socketSAFA;
t_queue* colaMDJ;

void entenderMensaje(int emisor, char header);

void escuchar(int servidor);

int main(void);

#endif /*DAM_H_*/
