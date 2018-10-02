/*
 ============================================================================
 Name        : FM9.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "FM9.h"
t_list* guardarDatos(char* datos){
	t_list* direccionesGuardadas;
	list_create(direccionesGuardadas);
	list_add(direccionesGuardadas,offset);
	waitMutex(&mutexStorage);
	memcpy(&storage, datos, strlen(datos) + 1);
	signalMutex(&mutexStorage);

	waitMutex(&mutexOffset);
	offset = offset + strlen(datos) + 1;
	signalMutex(&mutexOffset);
	return direccionesGuardadas;
}

respuestaDeCargaEnMemoria cargarDatosEnMemoria(char* datos){
	respuestaDeCargaEnMemoria respuesta;
	respuesta.listaDeDirecciones = guardarDatos(datos);
	//TODO pudoGuardarlo()
	if(1){
		respuesta.pudoGuardarlo = 1;
	}else{
		respuesta.pudoGuardarlo = 0;
	}
	printf("guardado en memoria: %s", datos);
	return respuesta; //pudo guardar. TODO hacer si tuvo un error return 0
}

void entenderMensaje(int emisor, int header){
	char identificado;
	char* datos;
	respuestaDeCargaEnMemoria respuestaDeCarga;
	u_int32_t id;
	u_int32_t tamanioBuffer;
	u_int32_t offset;
	u_int32_t sizeDeLaRespuesta;
	u_int32_t desplazamiento;
	u_int32_t tamanioPath;
	void* buffer;
		switch(header){
			case IDENTIFICARSE:
				identificado = deserializarChar(emisor);
				printf("identificado %c \n", identificado);
				switch(identificado){
					case CPU:
						socketCPU = emisor;
						break;
					case DAM:
						socketDAM = emisor;
						break;
					default:
						perror("no acepto a esta conexion");
				}
				printf("Se agrego a las conexiones %c \n" , identificado);
				break;

			case GUARDAR_DATOS:
				datos = deserializarString(emisor);
				respuestaDeCarga = cargarDatosEnMemoria(datos);
				//TODO enviar y serializar la estructura de guardado
				//enviarYSerializarInt(socketDAM, respuestaDeCarga, RESPUESTA_CARGA);
				tamanioBuffer = sizeof(u_int32_t) + sizeof(u_int32_t) + sizeof(u_int32_t)*list_size(respuestaDeCarga.listaDeDirecciones);
				buffer = asignarMemoria(tamanioBuffer);
				desplazamiento = 0;

				concatenarInt(buffer, &desplazamiento, respuestaDeCarga.pudoGuardarlo);
				if(respuestaDeCarga.pudoGuardarlo){
					concatenarInt(buffer, &desplazamiento, respuestaDeCarga.listaDeDirecciones->elements_count);
					for(int i = 0; i<respuestaDeCarga.listaDeDirecciones->elements_count ;i++){
						u_int32_t direccion = list_get(respuestaDeCarga.listaDeDirecciones, i);
						concatenarInt(buffer, &desplazamiento, direccion);
					}
				}
				enviarMensaje(socketDAM, buffer, tamanioBuffer);
				free(buffer);
				break;

			default:
				perror("Cualquiera ese header flaco");
		}
}

void init(){
	inicializarMutex(&mutexOffset);
	inicializarMutex(&mutexStorage);
	//TODO cargar storage
	storage = asignarMemoria(1000);
	offset = 0;
}

int main(void) {
	init();

	direccionServidor direccionFM9 = levantarDeConfiguracion(NULL, "PUERTO", ARCHIVO_CONFIGURACION);
	int servidor = crearServidor(direccionFM9.puerto, INADDR_ANY);

	parametrosEscucharClientes parametros;
	parametros.servidor = servidor;
	parametros.funcion = &entenderMensaje;
	pthread_t hiloAdministradorDeConexiones = crearHilo(&escucharClientes, &parametros);

	esperarHilo(hiloAdministradorDeConexiones);
	free(storage);
	return 0;
}
