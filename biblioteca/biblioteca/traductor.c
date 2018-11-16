#include "traductor.h"

char* traducirEstado(char estado){
	switch(estado){
		case NEW:
			return "NEW";
			break;
		case READY:
			return "READY";
			break;
		case BLOCKED:
			return "BLOCKED";
			break;
		case EXIT:
			return "EXIT";
			break;
		case EXECUTE:
			return "EXECUTE";
			break;
		case READY_PRIORIDAD:
			return "READY_PRIORIDAD";
			break;
		default:
			return "TOKEN INCORRECTO";
	}
}

char* traducirModulo(char modulo){
	switch(modulo){
		case SAFA:
			return "SAFA";
			break;
		case CPU:
			return "CPU";
			break;
		case MDJ:
			return "MDJ";
			break;
		case FM9:
			return "FM9";
			break;
		case DAM:
			return "DAM";
			break;
		default:
			return "TOKEN INCORRECTO";
	}
}

char* traducirHeaderExito(char header){
	switch(header){
		case GUARDADO_CON_EXITO_EN_MDJ:
			return "Guardado";
			break;
		case BORRADO_CON_EXITO_EN_MDJ:
			return "Borrado";
			break;
		case CREADO_CON_EXITO_EN_MDJ:
			return "Creado";
			break;
		default:
			return "El header no es uno de exito";
			break;
	}
}

