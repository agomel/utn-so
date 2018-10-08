#include "plp.h"

void planificadorALargoPlazo() {
	int a = 1;
	while (a) {
		waitSem(&semCantidadEnNew);
		log_info(logger, "Hay procesos en la cola new");

		DTB* dtb = obtenerPrimerDTBEnNew();

		cargarDummy(*dtb);

		signalSem(&cantidadTotalREADY);
	}
}

void cargarDummy(DTB dtb) {
	waitMutex(&mutexDummy);
	dtbDummy->flag = 0;
	dtbDummy->escriptorio = dtb.escriptorio;
	dtbDummy->id = dtb.id;
	dtbDummy->direccionesArchivos = dictionary_create();
	dtbDummy->estado = READY;
	agregarDTBALista(dtbDummy);
}

void ponerProcesoEnNew(char* escriptorio) {
	DTB* dtb = asignarMemoria(sizeof(DTB));
	*dtb = crearDTB(escriptorio);
	agregarDTBALista(dtb);
	signalSem(&semCantidadEnNew);
}

void enviarDTB(DTB dtb) {
	serializarYEnviarDTB(socketCPU, dtb);
}

void ponerEnReady(int idDTB) {
	cambiarEstado(idDTB, READY);
	signalSem(&cantidadTotalREADY);
}

void pasarDTBAExit(int idDTB) {
	DTB* dtb;
	signalSem(&gradoMultiprogramacion);
	cambiarEstado(idDTB, EXIT);
}

/*void manejarErrores(int idDTB,char* path,int error){
	cambiarEstado(idDTB, EXIT);
	signalSem(&gradoMultiprocesamiento);
	switch(error){
		case 10001:
			print("path inexistente de archivo %s", path);
			break;

		case 10002:
		case 20003:
			print("espacio insuficiente en FM9 para archivo %s", path);
			break;

		case 20001:
		case 30001:
		case 40001:
			print("El archivo no se encuentra abierto %s", path);
			break;

		case 20002:
		case 30002:
		case 40002:
			print("Fallo de segmento/memoria en path %s", path);
			break;
		case 30003:
			print("Espacio insuficiente en MDJ para archivo %s", path);
			break;
		case 30004:
			print("El archivo no existe en MDJ, fue borrado previamente MDJ path: %s", path);
			break;
		case 50001:
			print("Archivo ya existente: %s", path);
			break;
		case 50002:
			print("Espacio insuficiente en path: %s", path);
			break;
		default:
			perror("No reconozco el error, pero te termino el dtb");
	}
}*/
