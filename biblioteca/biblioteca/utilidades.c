#include "utilidades.h"

void* asignarMemoria(int cantidad){
	void* buffer = malloc(cantidad);
	if(buffer == NULL){
		printf("No hay espacio\n");
		exit(-99);
	}
	return buffer;
}

direccionServidor levantarDeConfiguracion(char* nombreIp, char* nombrePuerto, t_config* configuracion){
	direccionServidor direccion;
	if(nombreIp != NULL){
	direccion.ip = config_get_string_value(configuracion, nombreIp);
	}
	direccion.puerto = config_get_int_value(configuracion, nombrePuerto);

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

int listaContiene(t_list* list, char* string){
	bool isEqual(char* elem){
		return !strcmp(string, elem);
	}
	return list_any_satisfy(list, isEqual);
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

char* concatenar(char* str1, char* str2){
	char* concatenado = asignarMemoria(strlen(str1) + 1);
	memcpy(concatenado, str1, strlen(str1) + 1);
	string_append(&concatenado, str2);
	return concatenado;
}

void concatenarATexto(char** texto, char* adicional){
	string_append(texto, adicional);
}


char* byteToBit(char byte){
	char* bits = asignarMemoria(CHAR_BIT);
	for(int i = 0; i < CHAR_BIT; i++)
	    bits[i] = '0' + ((byte & (1 << i)) > 0);

	return bits;
}

char* bytesToBits(char* bytes){
	char* bits = asignarMemoria(CHAR_BIT * strlen(bytes) + 1);
	int desplazamiento = 0;
	for(int i = 0; i < strlen(bytes); i++){
		memcpy(bits + desplazamiento, byteToBit(bytes[i]), CHAR_BIT);
		desplazamiento += CHAR_BIT;
	}
	agregarBarraCero(bits);
	return bits;
}
