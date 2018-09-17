#include "plp.h"


t_queue* colaNEW;

void planificadorALargoPlazo(){
	u_int32_t a = 1;
	while(a){
		if(!queue_is_empty(colaNEW)){
			//TODO wait mutex cola new
			DTB dtb = queue_pop(colaNEW);
			//TODO signal mutex cola new
			//TODO operacion dummmy
			//wait lugares
			//wait mutex cola ready
			queue_push(colaREADY, dtb);
			//signal mutex cola ready
			//signal mensajes
		}
	}
}



