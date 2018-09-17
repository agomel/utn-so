#include "pcp.h"

void planificadorACortoPlazo(){
	u_int32_t a = 1;
	while(a){
		if(!queue_is_empty(colaREADY)){
			//wait mensaje
			//wait mutex cola ready
			DTB dtb = queue_pop(colaREADY);
			//signal mutex cola ready
			//signal lugares
			//TODO ejecutar el dtb
		}
	}
}


