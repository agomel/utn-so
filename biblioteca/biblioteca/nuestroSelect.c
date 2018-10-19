#include "nuestroSelect.h"

void agregarPedidoACola(char header,SocketEnSelect* socketEnSelect){
	OperacionSocket* operacion = asignarMemoria(sizeof(OperacionSocket));
	operacion->header = header;
	operacion->socket = socketEnSelect->conectado;
	waitMutex(socketEnSelect->select->mutexOperaciones);
	queue_push(socketEnSelect->select->colaOperaciones, operacion);
	signalMutex(socketEnSelect->select->mutexOperaciones);

}

void escucharCliente(SocketEnSelect* socketEnSelect){
	log_debug(socketEnSelect->select->logger, "Escuchando nuevo cliente en %d", socketEnSelect->conectado);
	while(1){
		char header;
		recibirMensaje(socketEnSelect->conectado, &header, sizeof(char));
		agregarPedidoACola(header, socketEnSelect);
		signalSem(socketEnSelect->select->semOperaciones);
		waitSem(socketEnSelect->select->semProductores);
	}
}

void consumirCola(Select* select){
	while(1){
		waitSem(select->semOperaciones);
		OperacionSocket* operacion = queue_pop(select->colaOperaciones);
		(*select->funcionEntenderMensaje)(operacion->socket, operacion->header);
		log_info(select->logger, "Terminada operacion");
		signalSem(select->semProductores);
	}
}

void aceptarClientes(Select* select){
	while(1){
		int socket = aceptarCliente(select->socket);
		char header;
		recibirMensaje(socket, &header, sizeof(char));
		int resultado = (*select->identificarse)(socket, header);
		if(resultado){
			SocketEnSelect* socketEnSelect = asignarMemoria(sizeof(SocketEnSelect));
			socketEnSelect->select = select;
			socketEnSelect->conectado = socket;
			crearHiloQueMuereSolo(&escucharCliente, socketEnSelect);
		}else{
			log_error(select->logger, "no se pudo identificar");
		}
	}
}
void realizarNuestroSelect(Select* select){
	empezarAEscuchar(select->socket, INADDR_ANY);
	crearHiloQueMuereSolo(&aceptarClientes, select);
	crearHiloQueMuereSolo(&consumirCola, select);
}

void freeSelect(Select* select){
	free(select->colaOperaciones);
	free(select->funcionEntenderMensaje);
	free(select->identificarse);
	free(select->logger);
	free(select->mutexOperaciones);
	free(select->semOperaciones);
	free(select->semProductores);
	free(select);
}
