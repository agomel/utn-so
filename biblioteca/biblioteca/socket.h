#ifndef SOCKET_H_
#define SOCKET_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>


int crearServidor(int puerto, char* ip);
static struct sockaddr_in crearDireccionServidor(int puerto, char* ip);
static int crearSocket();
int empezarAEscuchar(int servidor, int cantidadAEscuchar);

int aceptarCliente(int servidor);

int recibirMensaje(int socketEmisor, void* buffer, int bytesMaximos);

int conectarConServidor(int puerto, char* ip);

void enviarIdentificacion(char* nombre, int servidor);

void enviarMensaje(int socket, char* mensaje, int tamanio);
void agregarABolsa(int servidor,fd_set* bolsa);
int estaEnLaBolsa(int servidor,fd_set* bolsa);
int eliminarDeBolsa(int servidor,fd_set* bolsa);
void realizarSelectLectura(int sockets,fd_set* socketsDeLectura);

#endif /*SOCKET_H_*/
