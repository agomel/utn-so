#include "planificador.h"
#include "S-AFA.h"

void inicializarPlanificadores(){
	inicializarColas();
	inicializarSemaforos();
}

void inicializarColas(){
	colaNEW = queue_create();
	colaREADY = list_create();
	colaEsperandoDummy = list_create();
	colaEXECUTE = list_create();
	colaBLOCKED = list_create();
	colaEXIT = list_create();
}

void inicializarSemaforos(){
	inicializarMutex(&mutexNEW);
	inicializarMutex(&mutexREADY);
	inicializarMutex(&mutexEXECUTE);
	inicializarMutex(&mutexBLOCKED);
	inicializarMutex(&mutexEXIT);
	inicializarMutex(&mutexColaDummy);
	inicializarMutex(&mutexIdsDTB);
	contadorIds = 1;

	t_config* configuracion = config_create(ARCHIVO_CONFIGURACION);

	u_int32_t multiprogramacion = config_get_int_value(configuracion, "MULTIPROGRAMACION");
	inicializarSem(&gradoMultiprogramacion, multiprogramacion);
	inicializarSem(&cantidadTotalREADY, 0);
}

void pasarDTBAExit(u_int32_t idDTB, t_list* listaDeDTB){
	DTB* dtb;
	for(u_int32_t i = 0; i < listaDeDTB->elements_count; i++){
		dtb = list_get(listaDeDTB, i);
		if(dtb->id == idDTB){
			list_remove(listaDeDTB, i);
			signalSem(&gradoMultiprogramacion);
			break;//para cortar el for
		}
	}
	waitMutex(&mutexEXIT);
	list_add(colaEXIT, dtb);
	signalMutex(&mutexEXIT);
}

/*DTB* buscarDTB(int id, t_list listaDeDTB){
	bool esElDTB(DTB* dtb){
		return dtb->id == id;
	}
	return list_find(listaDeDTB, (void*)esElDTB);
}*///Lo comento, puede que en el futuro lo usemos o no...



