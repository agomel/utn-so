#ifndef SOCKET_H_
#define SOCKET_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>


int crearServidor(int puerto, char* ip, int cantidadAEscuchar);
struct sockaddr_in crearDireccionServidor(int puerto, char* ip);
void empezarAEscuchar(int servidor, int cantidadAEscuchar);

int conectarConCliente(int servidor);

int recibirMensaje(int socketEmisor, char** buffer, int bytesMaximos);

#endif /*SOCKET_H_*/
