#include "serializacion.h"

void handshake(int servidor, char modulo){
	int tamanioMensaje = sizeof(char)*2;
	void* mensaje = asignarMemoria(tamanioMensaje);
	int desplazamiento = 0;

	concatenarChar(mensaje, &desplazamiento, IDENTIFICARSE);
	concatenarChar(mensaje, &desplazamiento, modulo);

	enviarMensaje(servidor, mensaje, tamanioMensaje);
	free(mensaje);
}

void enviarYSerializarString(int destino, char* texto,char operacion){
	int tamanioTexto = strlen(texto) + 1;
	int tamanioMensaje = sizeof(char) + sizeof(int) + tamanioTexto;
	void* mensaje = asignarMemoria(tamanioMensaje);

	int desplazamiento = 0;

	concatenarChar(mensaje, &desplazamiento, operacion);
	concatenarString(mensaje, &desplazamiento, texto);

	enviarMensaje(destino, mensaje, tamanioMensaje);

	free(mensaje);
}
void enviarySerializarPathyCantidadDeLineas(int destino ,char* path, int cantidadDeLineas){
	void* buffer = asignarMemoria(sizeof(char) + sizeof(int) + (strlen(path)+1) + sizeof(int));
	int desplazamiento = 0;
	concatenarChar(buffer, &desplazamiento, CREAR_ARCHIVO);
	concatenarString(buffer, &desplazamiento, path);
	concatenarInt(buffer, &desplazamiento, cantidadDeLineas);
	enviarMensaje(destino, buffer, desplazamiento);
	free(buffer);
}

void enviarYSerializarStringSinHeader(int destino, char* texto){
	int tamanioTexto = strlen(texto) + 1;
	int tamanioMensaje = sizeof(int) + tamanioTexto;
	void* mensaje = asignarMemoria(tamanioMensaje);

	int desplazamiento = 0;

	concatenarString(mensaje, &desplazamiento, texto);

	enviarMensaje(destino, mensaje, tamanioMensaje);

	free(mensaje);
}

void enviarYSerializarInt(int destino, int numero,char operacion){
	int tamanioMensaje = sizeof(int) + sizeof(char);
	void* mensaje = asignarMemoria(tamanioMensaje);

	int desplazamiento = 0;

	concatenarChar(mensaje, &desplazamiento, operacion);
	concatenarInt(mensaje, &desplazamiento, numero);

	enviarMensaje(destino, mensaje, tamanioMensaje);

	free(mensaje);
}

void enviarYSerializarIntSinHeader(int destino, int numero){
	int tamanioMensaje = sizeof(int);
	void* mensaje = asignarMemoria(tamanioMensaje);

	int desplazamiento = 0;

	concatenarInt(mensaje, &desplazamiento, numero);

	enviarMensaje(destino, mensaje, tamanioMensaje);

	free(mensaje);
}

void concatenarChar(void* buffer, int* desplazamiento, char mensaje){
	memcpy(buffer + *desplazamiento, &mensaje, sizeof(char));
	*desplazamiento = *desplazamiento + sizeof(char);
}

void concatenarInt(void* buffer, int* desplazamiento, int numero){
	memcpy(buffer + *desplazamiento, &numero, sizeof(int));
	*desplazamiento = *desplazamiento + sizeof(int);
}

void concatenarString(void* buffer, int* desplazamiento, char* mensaje){
	concatenarInt(buffer, desplazamiento, strlen(mensaje) + 1);
	memcpy(buffer + *desplazamiento, mensaje, strlen(mensaje) + 1);
	*desplazamiento = *desplazamiento + strlen(mensaje) + 1;
}

void concatenarListaInt(void* buffer, int* desplazamiento, t_list* listaArchivos){
	concatenarInt(buffer, desplazamiento, listaArchivos->elements_count);
	for(int i = 0; i < (listaArchivos->elements_count); i++){
		concatenarInt(buffer, desplazamiento, list_get(listaArchivos, i));
	}
}

void concatenarDiccionario(void* buffer, int* desplazamiento, t_dictionary* diccionario){

	concatenarInt(buffer, desplazamiento, diccionario->elements_amount);
	void concatenarElemento(char* key, t_list* value){
		concatenarString(buffer, desplazamiento, key);
		concatenarListaInt(buffer, desplazamiento, value);
	}

	dictionary_iterator(diccionario, concatenarElemento);
}
void concatenarVoid(void* buffer, int* desplazamiento, void* mensaje, int tamanio){
	memcpy(buffer + *desplazamiento, mensaje, tamanio);
	*desplazamiento = *desplazamiento + tamanio;
}

char* deserializarString(int emisor){
	int tamanioMensaje = deserializarInt(emisor);
	char* mensaje = asignarMemoria(tamanioMensaje);
	recibirMensaje(emisor, mensaje, tamanioMensaje);
	printf("Recibi %s de parte de %d \n" , mensaje, emisor);
	return mensaje;
}

int deserializarInt(int emisor){
	int mensaje;
	recibirMensaje(emisor, &mensaje, sizeof(int));
	printf("Recibi %d de parte de %d \n" , mensaje, emisor);
	return mensaje;
}

char deserializarChar(int emisor){
	char mensaje;
	recibirMensaje(emisor, &mensaje, sizeof(char));
	printf("Recibi %c de parte de %d \n" , mensaje, emisor);
	return mensaje;
}

t_list* deserializarListaInt(int emisor){
	int elementosDeLalista = deserializarInt(emisor);
	t_list* respuesta = list_create();
	for(int i = 0; i < elementosDeLalista; i++){
		list_add(respuesta, deserializarInt(emisor));
	}
	return respuesta;
}

t_dictionary* deserializarDiccionario(int emisor){
	int cantidadElementos = deserializarInt(emisor);
	t_dictionary* respuesta = dictionary_create();
	for(int i = 0; i<cantidadElementos; i++){
			char* clave = deserializarString(emisor);
			t_list* valor = deserializarListaInt(emisor);
			dictionary_put(respuesta, clave, valor);
	}
	return respuesta;
}

int obtenerTamanioDiccionario(t_dictionary* diccionario){
	int respuesta = 0;
	void sumarPuntaje(char* key, t_list* value){
		respuesta += strlen(key) + 1 + sizeof(int);
		respuesta += sizeof(int) + sizeof(int)*(value->elements_count);
	}
	dictionary_iterator(diccionario, sumarPuntaje);
	return respuesta;
}

voidDeserealizado deserializarVoid(int emisor){
	voidDeserealizado mensajeADeserealizar;
	mensajeADeserealizar.tamanioMensaje = deserializarInt(emisor);
	char* mensaje = asignarMemoria(mensajeADeserealizar.tamanioMensaje);
	recibirMensaje(emisor, mensaje, mensajeADeserealizar.tamanioMensaje);
	mensajeADeserealizar.mensaje = *mensaje;
	free(mensaje);
	return mensajeADeserealizar;
}
