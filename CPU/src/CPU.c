#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "../../S-AFA/src/socket.c"

int main(void) {
	int cliente = conectarConServidor(20000, inet_addr("127.0.0.1"));

	while(1){
		char mensaje[1000];
		scanf("%s", mensaje);
		enviarMensaje(cliente, mensaje);
	}
	return 0;
}
