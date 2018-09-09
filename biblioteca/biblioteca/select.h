#ifndef SELECT_H_
#define SELECT_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


int recibirConexionesYMensajes(int servidor,void (*funcionEntenderMensaje)(char*));

#endif /*SELECT_H_*/
