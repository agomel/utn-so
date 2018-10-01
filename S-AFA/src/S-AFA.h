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

u_int32_t socketCPU;
u_int32_t socketDAM;

void entenderMensaje(int emisor, char header);

int main(void);

#endif /*S-AFA_H_*/
