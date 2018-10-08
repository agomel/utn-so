#include "plp.h"

void planificadorALargoPlazo() {
	u_int32_t a = 1;
	while (a) {
		waitSem(&semCantidadEnNew);
		printf("hay procesos en la cola new\n");
		waitMutex(&mutexNEW);
		DTB* dtb = queue_pop(colaNEW);
		signalMutex(&mutexNEW);

		waitMutex(&mutexColaDummy);
		list_add(colaEsperandoDummy, dtb);
		signalMutex(&mutexColaDummy);
		dtb->estado = ESPERANDO_DUMMY;
		cargarDummy(*dtb);

		waitMutex(&mutexREADY);
		list_add(colaREADY, dtbDummy);
		signalMutex(&mutexREADY);
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
}

void ponerProcesoEnNew(char* escriptorio) {
	DTB* dtb = asignarMemoria(sizeof(DTB));
	*dtb = crearDTB(escriptorio);

	waitMutex(&mutexNEW);
	queue_push(colaNEW, dtb);
	signalMutex(&mutexNEW);

	waitMutex(&mutexListaDTBs);
	list_add(listaDeTodosLosDTBs, dtb);
	signalMutex(&mutexListaDTBs);
	signalSem(&semCantidadEnNew);
}

void enviarDTB(DTB dtb) {
	serializarYEnviarDTB(socketCPU, dtb);
}

void ponerEnReady(DTB* dtb) {
	waitSem(&gradoMultiprogramacion);
	waitMutex(&mutexREADY);
	list_add(colaREADY, dtb);
	signalMutex(&mutexREADY);
	signalSem(&cantidadTotalREADY);
	dtb->estado = READY;
	t_list* lista = dictionary_get(dtb->direccionesArchivos, dtb->escriptorio);
	printf("DTB al ponerlo en READY tiene escriptorio %s, quantum: %d, y el escriptorio se encuentra en la posicion%d \n"
			, dtb->escriptorio, dtb->quantum, list_get(lista, 0));
}

void pasarDTBAExit(u_int32_t idDTB, t_list* listaDeDTB) {
	DTB* dtb;
	for (u_int32_t i = 0; i < listaDeDTB->elements_count; i++) {
		dtb = list_get(listaDeDTB, i);
		if (dtb->id == idDTB) {
			list_remove(listaDeDTB, i);
			signalSem(&gradoMultiprogramacion);
			break;
		}
	}
	waitMutex(&mutexEXIT);
	list_add(colaEXIT, dtb);
	signalMutex(&mutexEXIT);
	dtb->estado = EXIT;
}
