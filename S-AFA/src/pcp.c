#include "pcp.h"

void planificadorACortoPlazo(){
	u_int32_t a = 1;
	while(a){
		if(!list_is_empty(colaREADY)){
			printf("tamanio cola ready %d \n",colaREADY->elements_count);
			//algoritmo para elegir
			int indexElegido = 0;
			//El list remove hace un get y lo borra de la lista
			waitSem(&cantidadTotalREADY);
			waitMutex(&mutexREADY);
			DTB* dtb;
			dtb = list_remove(colaREADY, 0);
			dtb->quantum = -1;
			signalMutex(&mutexREADY);

			serializarYEnviarDTB(socketCPU, *dtb);

			if(dtb->flag==0){
				//Es el dummy, lo desbloqueo para que se pueda volver a usar
				signalMutex(&mutexDummy);
			}
		}
	}
}

void pasarDTBAReady(DTB* dtb){
	//TODO hacer el obtenerCola diferenciando si es new o sino llamar a obtenercola
	//TODO sacarlo de la cola de bloqueados y ponerla en ready
	//TODO hacer bloquear dtb que lo saque de cualquier cola y lo ponga en bloqued
}

void desbloquearDTB(DTB* dtb){
	if(dtb->flag == 0){
		signalMutex(&mutexDummy);
	}else{
		pasarDTBAReady(dtb);
	}
}

t_list* obtenerCola(char estado){
	switch(estado){
		case NEW:
			return col
	}
}

