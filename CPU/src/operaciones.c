#include "operaciones.h"

void bloquearDTB(DTB* dtbRecibido){
	//Bloquear dtb
	log_info(logger, "Bloquear DTB");
	serializarYEnviarDTB(socketSAFA, *dtbRecibido, logger, BLOQUEAR_DTB);
	enviarYSerializarIntSinHeader(socketSAFA, sentencias);
	char continuar = deserializarChar(socketSAFA);
	//continuar
}
char abrir(char* lineaEjecutando, DTB* dtbRecibido){
	char* pathRecibido = asignarMemoria(strlen(lineaEjecutando)-5);
	pathRecibido = string_substring_from(lineaEjecutando, 6);
	string_append(&pathRecibido, "\0");
	//Corrobora si ya esta abierto
	if(listaContiene(dtbRecibido->listaDeArchivos, pathRecibido)){
		return 's';
	}else {
		bloquearDTB(dtbRecibido);
		int tamanioPathEscriptorioACargar;
		int tamanioBuffer2;
		int desplazamiento = 0;
		void* buffer2;
		tamanioPathEscriptorioACargar = strlen(pathRecibido) + 1;
		tamanioBuffer2 = sizeof(char) + tamanioPathEscriptorioACargar + sizeof(int)*3;
		buffer2 = asignarMemoria(tamanioBuffer2);

		concatenarChar(buffer2, &desplazamiento, CARGAR_ESCRIPTORIO_EN_MEMORIA);
		concatenarInt(buffer2, &desplazamiento, dtbRecibido->id);
		concatenarString(buffer2, &desplazamiento, pathRecibido);
		concatenarInt(buffer2, &desplazamiento, 1); //No DUMMY
		enviarMensaje(socketDIEGO, buffer2, tamanioBuffer2);
		free(buffer2);
		free(pathRecibido);
		return 'b';
	}
}

char asignar(char* lineaEjecutando, DTB* dtbRecibido){
	char* parametros = string_substring_from(lineaEjecutando, 8);
	char** pathYNumeroLinea= string_n_split(parametros, 3, " ");
	char* path = pathYNumeroLinea[0];
	int numeroDeLinea = atoi(pathYNumeroLinea[1]);
	char* datos = pathYNumeroLinea[2];
	if(listaContiene(dtbRecibido->listaDeArchivos, path)){
		//Esta abierto

		void* buffer = asignarMemoria(sizeof(char) + sizeof(int)*4 + (strlen(path)+1) + strlen(datos) + 1);
		int desplazamiento = 0;

		concatenarChar(buffer, &desplazamiento, ASIGNAR_DATOS);
		concatenarInt(buffer, &desplazamiento, dtbRecibido->id);
		concatenarString(buffer, &desplazamiento, path);
		concatenarInt(buffer, &desplazamiento, numeroDeLinea);
		concatenarString(buffer, &desplazamiento, datos);

		enviarMensaje(socketFM9, buffer, desplazamiento);

		free(buffer);

		int respuestaAsignado = deserializarInt(socketFM9);
		if(respuestaAsignado == 0){
			return 's';
		}else {
			return 'a';
		}

	}else{
		//No esta abierto ese archivo
		log_info(logger, "No esta abierto el archivo %s, no se hace asignar y se aborta el dtb", path);
		return 'a';
	}
}
char wait(char* lineaEjecutando, DTB* dtbRecibido){
	char* recursoRecibido = asignarMemoria(strlen(lineaEjecutando)-4);
	recursoRecibido = string_substring_from(lineaEjecutando, 5);
	enviarYSerializarString(socketSAFA, recursoRecibido, RETENCION_DE_RECURSO);
	enviarYSerializarIntSinHeader(socketSAFA, dtbRecibido->id);
	char seguirConEjecucion = deserializarChar(socketSAFA);
	free(recursoRecibido);
	if(seguirConEjecucion == LIBERAR_DTB_DE_EJECUCION){
		return 'b';
	}else{
		return 's';
	}
}

char signal(char* lineaEjecutando, DTB* dtbRecibido){
	char* recursoRecibido = asignarMemoria(strlen(lineaEjecutando)-6);
	recursoRecibido = string_substring_from(lineaEjecutando, 7);
	enviarYSerializarString(socketSAFA, recursoRecibido, LIBERAR_RECURSO);
	char seguirConEjecucion = deserializarChar(socketSAFA);
	free(recursoRecibido);
	if(seguirConEjecucion == LIBERAR_DTB_DE_EJECUCION){
		return 'b';
	}else{
		return 's';
	}
}

char flush(char* lineaEjecutando, DTB* dtbRecibido){
	char* pathRecibido = asignarMemoria(strlen(lineaEjecutando)-5);
	pathRecibido = string_substring_from(lineaEjecutando, 6);
	if(listaContiene(dtbRecibido->listaDeArchivos, pathRecibido)){
		bloquearDTB(dtbRecibido);
		//Esta abierto
		void* buffer;
		int desplazamiento = 0;
		int tamanioBuffer = sizeof(char) + (strlen(pathRecibido)+1) + sizeof(int)*2;
		buffer = asignarMemoria(tamanioBuffer);

		concatenarChar(buffer, &desplazamiento, GUARDAR_ESCRIPTORIO);
		concatenarInt(buffer, &desplazamiento, dtbRecibido->id);
		concatenarString(buffer, &desplazamiento, pathRecibido);
		enviarMensaje(socketDIEGO, buffer, tamanioBuffer);
		free(buffer);
		free(pathRecibido);
		return 'b';
	}else{
		log_error(logger, "El DTB %d no tiene el archivo %s abierto", dtbRecibido->id, pathRecibido);
		//No esta abierto ese archivo
		free(pathRecibido);
		return 'a';
	}
}

void eliminarArchivo(char* pathRecibido, DTB* dtbRecibido){
	bool coincideNombre(char* pathAComparar){
	if(strcmp(pathAComparar, pathRecibido) == 0){
		return true;
	}
	return false;
	}
	list_remove_and_destroy_by_condition(dtbRecibido->listaDeArchivos, coincideNombre, free);
}
char cerrar(char* lineaEjecutando, DTB* dtbRecibido){
	char* pathRecibido = asignarMemoria(strlen(lineaEjecutando)-5);
	pathRecibido = string_substring_from(lineaEjecutando, 6);
	if(listaContiene(dtbRecibido->listaDeArchivos, pathRecibido)){
		//Esta abierto
		void* buffer;
		int desplazamiento = 0;
		int tamanioBuffer = sizeof(char) + sizeof(int) + strlen(pathRecibido) + 1;
		concatenarChar(buffer, &desplazamiento, LIBERAR_MEMORIA);
		concatenarString(buffer, &desplazamiento, pathRecibido);
		buffer = asignarMemoria(tamanioBuffer);

		enviarMensaje(socketFM9, buffer, tamanioBuffer);
		free(buffer);

		eliminarArchivo(pathRecibido, dtbRecibido);
		free(pathRecibido);
		return 'b';
	}else{
		//No esta abierto ese archivo
		free(pathRecibido);
		return 'a';
	}
}

char crear(char* lineaEjecutando, DTB* dtbRecibido){
	bloquearDTB(dtbRecibido);
	char* parametros = string_substring_from(lineaEjecutando, 6);
	char** pathYCantLineas = string_n_split(parametros, 2, ' ');
	char* path = pathYCantLineas[0];
	int cantidadDeLineas = pathYCantLineas[1];
	enviarySerializarPathyTamanioArchivo(socketDIEGO, path, cantidadDeLineas);
	return 'b';
}

char borrar(char* lineaEjecutando, DTB* dtbRecibido){
	bloquearDTB(dtbRecibido);

	char* pathRecibido = asignarMemoria(strlen(lineaEjecutando)-5);
	pathRecibido = string_substring_from(lineaEjecutando, 6);
	enviarYSerializarString(socketDIEGO, pathRecibido, BORRAR_ARCHIVO);
	free(pathRecibido);
	return 'b';
}
