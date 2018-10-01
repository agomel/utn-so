#include "plp.h"

void planificadorALargoPlazo(){
	u_int32_t a = 1;
	while(a){
		if(!queue_is_empty(colaNEW)){
			printf("hay procesos en la cola new\n");
			waitMutex(&mutexNEW);
			DTB* dtb = queue_pop(colaNEW);
			signalMutex(&mutexNEW);

			wait(mutexColaDummy);
			list_add(colaEsperandoDummy, dtb);
			signal(mutexColaDummy);

			cargarDummy(dtb);

			waitSem(&gradoMultiprogramacion); //TODO hacer el signal cuando el proceso va a exit
			waitMutex(&mutexREADY);
			list_add(colaREADY, &dtbDummy);
			signalMutex(&mutexREADY);
			signalSem(&cantidadTotalREADY);
		}
	}
}

/*void dtbListo(DTBListo datos){
	//Busco en la lista el dtb con el id de datos dtb
	DTB dtb;
	waitSem(&espacioDisponibleREADY);
	waitMutex(&mutexREADY);
	queue_push(colaREADY, dtb);
	signalMutex(&mutexREADY);
	signalSem(&cantidadTotalREADY);
}*/
void cargarDummy(DTB dtb){
	waitMutex(&mutexDummy);
	dtbDummy.flag = 0;
	dtbDummy.escriptorio = dtb.escriptorio;
	dtbDummy.id = dtb.id;
	//serializarYEnviarDTB(socketCPU, dtbDummy);
}

void ponerProcesoEnNew(char* escriptorio){
	DTB* dtb = asignarMemoria(sizeof(DTB));
	*dtb = crearDTB(escriptorio);
	waitMutex(&mutexNEW);
	queue_push(colaNEW, dtb);
	signalMutex(&mutexNEW);
	free(dtb);
}

void enviarDTB(DTB dtb){
	serializarYEnviarDTB(socketCPU, dtb);
}

//TODO no recibe int recibe estructura de guardado
void ponerEnReadyProcesoDummyOk(u_int32_t idDTB){
	DTB* dtb;
	int index = 0;
	int salir = 0;
	while(index<colaEsperandoDummy->elements_count && !salir){
		dtb = list_get(colaEsperandoDummy, index);
		if(dtb->id == idDTB){
			salir = 1;
			list_remove(colaEsperandoDummy,index);
		}
		index++;
	}
	//TODO esta lista es imaginaria. Cambiar el uint por lista
	dtb->tablaDireccionesArchivos = 2;
	waitSem(&gradoMultiprogramacion); //TODO hacer el signal cuando el proceso va a exit
	waitMutex(&mutexREADY);
	list_add(colaREADY, &dtb);
	signalMutex(&mutexREADY);
	signalSem(&cantidadTotalREADY);
}
