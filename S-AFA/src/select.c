
/*
** selectserver.c -- servidor de chat multiusuario
*/
#include "select.h"
#define PORT 20001   // puerto en el que escuchamos
int administradorDeConexiones(){
	fd_set bolsaDeTodosLosSockets;   // conjunto maestro de sockets
	fd_set socketsDeLectura; // conjunto temporal de sockets para select()

	int numeroMaximoDeSockets;        // número máximo de sockets
	int yoComoServidor;     // descriptor de socket a la escucha
	int cliente;        // descriptor de socket de nueva conexión aceptada

	char* buffer;    // buffer para datos del cliente
	int bytesRecibidos;
	int yes=1;        // para setsockopt() SO_REUSEADDR, más abajo

	int addrlen;
	int socketDeLaBolsa;
	FD_ZERO(&bolsaDeTodosLosSockets);    // borra los conjuntos maestro y temporal
	FD_ZERO(&socketsDeLectura);

	yoComoServidor=crearServidor(PORT,INADDR_ANY);

	// escuchar
	if (empezarAEscuchar(yoComoServidor, 10) == -1) {
		perror("listen");
		exit(1);
	}

	// añadir yoComoServidor al conjunto maestro
	FD_SET(yoComoServidor, &bolsaDeTodosLosSockets);

	// seguir la pista del socket mayor
	numeroMaximoDeSockets = yoComoServidor; // por ahora es éste

	// bucle principal
	for(;;) {
	socketsDeLectura = bolsaDeTodosLosSockets;
	if (select(numeroMaximoDeSockets+1, &socketsDeLectura, NULL, NULL, NULL) == -1) {
		perror("select");
		exit(1);
	}

	// explorar conexiones existentes en busca de datos que leer
	for(socketDeLaBolsa = 0; socketDeLaBolsa <= numeroMaximoDeSockets; socketDeLaBolsa++) {
		buffer=asignarMemoria(5);
		if (FD_ISSET(socketDeLaBolsa, &socketsDeLectura)) { // ¡¡tenemos datos!!
			if (socketDeLaBolsa == yoComoServidor) {
				// agregar cliente
				cliente=aceptarCliente(yoComoServidor);
				if (cliente != -1) {
					FD_SET(cliente, &bolsaDeTodosLosSockets); // añadir al conjunto maestro
					if (cliente > numeroMaximoDeSockets) {    // actualizar el máximo
						numeroMaximoDeSockets = cliente;
					}
					printf("Recibi una nueva conexion en %d\n", cliente);
				}
			} else {
				// escuchar a cliente
				bytesRecibidos = recibirMensaje(socketDeLaBolsa,&buffer,4);
				if (bytesRecibidos <= 0) {
					close(socketDeLaBolsa); // bye!
					FD_CLR(socketDeLaBolsa, &bolsaDeTodosLosSockets); // eliminar del conjunto maestro
				} else {
					// tenemos datos de algún cliente
					printf("Me llegaron %d bytes con %s \n", bytesRecibidos, buffer);
					enviarMensaje(socketDeLaBolsa,"gracias por la info");
				}
			}
		}
		free(buffer);
	}
}

	return 0;
}
