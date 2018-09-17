#include "dtb.h"

u_int32_t obtenerId(){
	//TODO wait mutex
	u_int32_t id = contadorIds;
	contadorIds++;
	//TODO signal mutex
	return id;
}

DTB crearDTB (char* parametro){
	DTB dtb;

	dtb.escriptorio = parametro;
	dtb.flag = 0;
	dtb.id = obtenerId();
	//TODO poner valor valido en programCounter
	dtb.programCounter = -1;
	//TODO poner valor valido en tablaDireccionesArchivos
	dtb.tablaDireccionesArchivos = -1;

	return dtb;
}
