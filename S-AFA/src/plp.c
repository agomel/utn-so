#include "plp.h"

void planificadorALargoPlazo() {
	int a = 1;
	while (a) {
		waitSem(&semCantidadEnNew);
		printf("hay procesos en la cola new\n");

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

void manejarErrores(int idDTB,char* path,int error){
	cambiarEstado(idDTB, EXIT);
	switch(error){
		case 10001:
			perror("path inexistente de archivo %s", path);
			break;

		case 10001:
			perror("path inexistente de archivo %s", path);
			break;

		case 10001:
			perror("path inexistente de archivo %s", path);
			break;

		case 10001:
			perror("path inexistente de archivo %s", path);
			break;
	}
}
