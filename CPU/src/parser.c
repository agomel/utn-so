#include "parser.h"


int entendiendoLinea(char* lineaEjecutando, DTB* dtbRecibido){
	//Devuelve 1 si lo llama al diego para que el safa lo bloquee
	//sino devuelve 0 para que siga ejecutando el escriptorio
	//el numero de error si hay que abortar el G.DT
	if(string_starts_with(lineaEjecutando, "abrir")){
		log_info(logger, "Ejecutando instruccion abrir");
		return abrir(lineaEjecutando, dtbRecibido);

	}else if(string_starts_with(lineaEjecutando, "concentrar")){
		log_info(logger, "Ejecutando instruccion concentrar");
		return 0;

	}else if(string_starts_with(lineaEjecutando, "asignar")){
		//Asignar
		log_info(logger, "Ejecutando instruccion asignar");
		return asignar(lineaEjecutando, dtbRecibido);
	}else if(string_starts_with(lineaEjecutando, "wait")){
		log_info(logger, "Ejecutando instruccion wait");
		return wait(lineaEjecutando, dtbRecibido);

	}else if(string_starts_with(lineaEjecutando, "signal")){
		log_info(logger, "Ejecutando instruccion signal");
		return signalion(lineaEjecutando, dtbRecibido);

	}else if(string_starts_with(lineaEjecutando, "flush")){
		log_info(logger, "Ejecutando instruccion flush");
		return flush(lineaEjecutando, dtbRecibido);

	}else if(string_starts_with(lineaEjecutando, "close")){
		log_info(logger, "Ejecutando instruccion close");
		return cerrar(lineaEjecutando, dtbRecibido);
	}else if(string_starts_with(lineaEjecutando, "crear")){
		log_info(logger, "Ejecutando instruccion crear");
		return crear(lineaEjecutando, dtbRecibido);
	}else if(string_starts_with(lineaEjecutando, "borrar")){
		log_info(logger, "Ejecutando instruccion borrar");
		return borrar(lineaEjecutando, dtbRecibido);
	}else
		log_error(logger, "No deberia haber llegado aca, %s tendria que ser una palabra reservada conocida", lineaEjecutando);
}

