#include "utilidades.h"

void* asignarMemoria(int cantidad){
	void* buffer = malloc(cantidad);
	if(buffer == NULL){
		printf("No hay espacio\n");
		exit(-99);
	}
	return buffer;
}
direccionServidor levantarDeConfiguracion(char* nombreIp, char* nombrePuerto, char* rutaArchivo){
	t_config* configuracion = config_create(rutaArchivo);
	direccionServidor direccion;
	if(nombreIp != NULL){
	direccion.ip = config_get_string_value(configuracion, nombreIp);
	}
	direccion.puerto = config_get_int_value(configuracion, nombrePuerto);

	free(configuracion->properties);
	free(configuracion->path);
	free(configuracion);
	return direccion;
}

int escucharClientes(parametrosEscucharClientes* parametros) {
	log_debug(parametros->logger, "Servidor a escuchar %d\n", parametros->servidor);
	empezarAEscuchar(parametros->servidor, 100);
	recibirConexionesYMensajes(parametros->servidor, parametros->funcion);
}

char* intToString(int numero){
	return string_from_format("%d", numero);
}


char* nombreEstado(char estado){
	switch(estado){
	case NEW: return "NEW";
		break;
	case READY: return "READY";
	break;
	case BLOCKED: return "BLOCKED";
	break;
	case EXIT: return "EXIT";
		break;
	case EXECUTE: return "EXECUTE";
	break;
	case READY_PRIORIDAD: return "READY_PRIORIDAD";
		break;
	default: return "LISTA NO RECONOCIDA";
	break;
	}
}
