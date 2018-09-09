#ifndef SOCKET_H_
#define SOCKET_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>


int crearServidor(int puerto, char* ip, int cantidadAEscuchar);
static struct sockaddr_in crearDireccionServidor(int puerto, char* ip);
static int crearSocket();
static void empezarAEscuchar(int servidor, int cantidadAEscuchar);

int aceptarCliente(int servidor);

int recibirMensaje(int socketEmisor, char** buffer, int bytesMaximos);

int conectarConServidor(int puerto, char* ip);

void enviarMensaje(int socket, char* mensaje);

#endif /*SOCKET_H_*/
