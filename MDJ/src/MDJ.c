/*
 ============================================================================
 Name        : MDJ.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <biblioteca/utilidades.h>
#include <biblioteca/socket.h>
#include <biblioteca/select.h>
#include <biblioteca/hilos.h>


u_int32_t socketDAM;


void escuchar(int servidor){
	int a = 1;
	while(a){
		char* buffer=asignarMemoria(150);
		int bytesRecibidos = recibirMensaje(servidor,&buffer,150);
		if(bytesRecibidos <= 0){
			a = 0;
		}
		printf("%s \n", buffer);
		free(buffer);
	}
}

int main(void) {
	direccionServidor direccionMDJ = levantarDeConfiguracion(NULL, "PUERTO", ARCHIVO_CONFIGURACION);
	int servidor = crearServidor(direccionMDJ.puerto, INADDR_ANY);

	socketDAM = aceptarCliente(servidor);
	empezarAEscuchar(servidor, 1);
	pthread_t hiloEscuchadorDAM = crearHilo(&escuchar, socketDAM);

	return 0;
}
