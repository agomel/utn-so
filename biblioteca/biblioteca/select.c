
/*
** selectserver.c -- servidor de chat multiusuario
*/
#include "select.h"
int recibirConexionesYMensajes(int servidor){
	fd_set bolsaDeTodosLosSockets;   // conjunto maestro de sockets
	fd_set socketsDeLectura; // conjunto temporal de sockets para select()

	int numeroMaximoDeSockets;        // número máximo de sockets
	int cliente;        // descriptor de socket de nueva conexión aceptada

	char* buffer;    // buffer para datos del cliente
	int bytesRecibidos;
	int yes=1;        // para setsockopt() SO_REUSEADDR, más abajo

	int addrlen;
	int socketDeLaBolsa;
	FD_ZERO(&bolsaDeTodosLosSockets);    // borra los conjuntos maestro y temporal
	FD_ZERO(&socketsDeLectura);



	// añadir servidor al conjunto maestro
	agregarABolsa(servidor, &bolsaDeTodosLosSockets);

	// seguir la pista del socket mayor
	numeroMaximoDeSockets = servidor; // por ahora es éste

	// bucle principal
	while(1) {
	socketsDeLectura = bolsaDeTodosLosSockets;
	realizarSelectLectura(numeroMaximoDeSockets+1, &socketsDeLectura);

	// explorar conexiones existentes en busca de datos que leer
	for(socketDeLaBolsa = 0; socketDeLaBolsa <= numeroMaximoDeSockets; socketDeLaBolsa++) {
		if (estaEnLaBolsa(socketDeLaBolsa, &socketsDeLectura)) { // ¡¡tenemos datos!!
			if (socketDeLaBolsa == servidor) {
				// agregar cliente
				cliente=aceptarCliente(servidor);
				if (cliente != -1) {
					agregarABolsa(cliente, &bolsaDeTodosLosSockets); // añadir al conjunto maestro
					if (cliente > numeroMaximoDeSockets) {    // actualizar el máximo
						numeroMaximoDeSockets = cliente;
					}
					printf("Recibi una nueva conexion en %d\n", cliente);
				}
			} else {
				// escuchar a cliente
				buffer=asignarMemoria(150);
				bytesRecibidos = recibirMensaje(socketDeLaBolsa,&buffer,149);
				if (bytesRecibidos <= 0) {
					close(socketDeLaBolsa); // bye!
					eliminarDeBolsa(socketDeLaBolsa, &bolsaDeTodosLosSockets); // eliminar del conjunto maestro
				} else {
					// tenemos datos de algún cliente
					printf("Me llegaron %d bytes con %s \n", bytesRecibidos, buffer);
					enviarMensaje(socketDeLaBolsa,"gracias por la info");
				}
				free(buffer);
			}
		}
	}
}

	return 0;
}
