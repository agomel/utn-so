#include "planificador.h"

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
}

void inicializarSemaforos(){
	inicializarMutex(&mutexNEW);
	inicializarMutex(&mutexREADY);

	//TODO pasarle el espacio disponible en READY (grado de multiprogramacion) por parametro
	inicializarSem(&gradoMultiprogramacion, 3);
	inicializarSem(&cantidadTotalREADY, 0);
}

void pasarDTBAExit(u_int32_t idDTB){
	/**
		* @NAME: list_find
		* @DESC: Retorna el primer valor encontrado, el cual haga que condition devuelva != 0
	*/
	void *list_find(t_list *, bool(*closure)(void*));

	list_find(colaEsperandoDummy, )
}

int buscarDTB(int id, t_list listaDeDTB){

	bool esElDTB(DTB* dtb){
		return dtb->id == id;
	}

	DTB* dtb

}



