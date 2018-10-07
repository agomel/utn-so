
#include <biblioteca/socket.h>
#include <biblioteca/hilos.h>
#include <biblioteca/utilidades.h>
#include <biblioteca/dtb.h>

int socketDIEGO;
int socketFM9;
int socketSAFA;
void consola(int servidor){
	while(1){
		char* texto = asignarMemoria(1000);
		scanf("%s", texto);
		enviarYSerializarString(servidor, texto, MANDAR_TEXTO);
		free(texto);
	}
}
void entenderMensaje(char* lineaEjecutando){
	if(strncmp(lineaEjecutando, "abrir", 5) == 0){
		//Abrir
	}else if(strncmp(lineaEjecutando, "concentrar", 10) == 0){
		//Concentrar
	}else if(strncmp(lineaEjecutando, "asignar", 7) == 0){
		//Asignar
	}else if(strncmp(lineaEjecutando, "wait", 4) == 0){
		//Wait
	}else if(strncmp(lineaEjecutando, "signal", 6) == 0){
		//Signal
	}else if(strncmp(lineaEjecutando, "flush", 5) == 0){
		//Flush
	}else if(strncmp(lineaEjecutando, "close", 5) == 0){
		//Close
	}else if(strncmp(lineaEjecutando, "crear", 5) == 0){
		//Crear
	}else if(strncmp(lineaEjecutando, "borrar", 6) == 0){
		//Borrar
	}
}
void pedirCosasDelFM9(DTB dtbRecibido){
	void* buffer;
	u_int32_t desplazamiento = 0;
	u_int32_t tamanioBuffer = sizeof(char) + sizeof(u_int32_t) + list_size(dtbRecibido.tablaDireccionesArchivos);
	buffer = asignarMemoria(tamanioBuffer);

	concatenarChar(buffer, &desplazamiento, TRAER_LINEA_ESCRIPTORIO);
	concatenarInt(buffer, &desplazamiento, dtbRecibido.programCounter);
	concatenarListaInt(buffer, &desplazamiento, dtbRecibido.tablaDireccionesArchivos);
	enviarMensaje(socketFM9, buffer, tamanioBuffer);
}
void escuchar(int servidor){
	int a = 1;
	u_int32_t tamanioPathEscriptorio;
	u_int32_t tamanioBuffer;
	void* buffer;
	u_int32_t desplazamiento;
	u_int32_t sizeDelEscriptorio = 10;

	while(a){
		DTB dtbRecibido;
		char header = deserializarChar(servidor);
			switch(header){
				case ENVIAR_DTB:
					printf("llego un dtb\n");
					dtbRecibido = deserializarDTB(servidor);
					if(dtbRecibido.flag == 0){
						//Es el dummy
						tamanioPathEscriptorio = strlen(dtbRecibido.escriptorio) + 1;
						tamanioBuffer = sizeof(char) + tamanioPathEscriptorio + sizeof(u_int32_t)*4;
						buffer = asignarMemoria(tamanioBuffer);
						desplazamiento = 0;

						concatenarChar(buffer, &desplazamiento, CARGAR_ESCRIPTORIO);
						concatenarString(buffer, &desplazamiento, dtbRecibido.escriptorio);
						concatenarInt(buffer, &desplazamiento, dtbRecibido.id);
						concatenarInt(buffer, &desplazamiento, 0); //Offset (Desde el principio)
						concatenarInt(buffer, &desplazamiento, -1); //size  (Hasta el final)

						enviarMensaje(socketDIEGO, buffer, tamanioBuffer);
						free(buffer);
						desplazamiento = 0;
						concatenarChar(buffer, &desplazamiento, DESBLOQUEAR_DTB);
						buffer = asignarMemoria(sizeof(char));
						enviarMensaje(socketSAFA, buffer, sizeof(char));
						serializarYEnviarDTB(socketSAFA, dtbRecibido);
					}else{
						//No es el dummy
						if(dtbRecibido.quantum > 0){
							for (u_int32_t q = 0; q < dtbRecibido.quantum; ++q){
								pedirCosasDelFM9(dtbRecibido);
								char* lineaAEjecutar = deserializarString(socketFM9);
								if(lineaAEjecutar[0] == '#'){
									//MensajeNano: Fijarse si las lineas de mas hay que ignorarlas o si directamente no me las mandan
									q--;
								}else if(lineaAEjecutar[0] == '@'){
									//Fin de archivo
									q--;
									dtbRecibido.quantum = dtbRecibido.quantum - q;
									break;
								}else if(lineaAEjecutar[0] == '!'){
									//MensajeNano: Hubo un acceso invalido o error en el FM9. Mandarle al Safa que ponga el dtb en exit
								}else{
									entenderMensaje(lineaAEjecutar);
								}
							//MensajeNano: Enviarle al Safa BLOQUEAR_DTB con el dtbrecibido
							printf("Ejecutar dtb");
							}
						}else{
							while()
							pedirCosasDelFM9(dtbRecibido);
							char* lineaAEjecutar = deserializarString(socketFM9);
							entenderMensaje(lineaAEjecutar);
						}
					}
					break;
		default:
					perror("Cualquiera ese header flaco");
					}
	}
}

int main(void) {
	direccionServidor direccionSAFA = levantarDeConfiguracion("IP_SAFA", "PUERTO_SAFA", ARCHIVO_CONFIGURACION);
	socketSAFA = conectarConServidor(direccionSAFA.puerto, inet_addr(direccionSAFA.ip));
	handshake(socketSAFA, CPU);
	printf("Socket del safa %d \n", socketSAFA);

	direccionServidor direccionFM9 = levantarDeConfiguracion("IP_FM9", "PUERTO_FM9", ARCHIVO_CONFIGURACION);
	socketFM9 = conectarConServidor(direccionFM9.puerto, inet_addr(direccionFM9.ip));
	handshake(socketFM9, CPU);

	direccionServidor direccionDIEGO = levantarDeConfiguracion("IP_DIEGO", "PUERTO_DIEGO", ARCHIVO_CONFIGURACION);
	socketDIEGO= conectarConServidor(direccionDIEGO.puerto, inet_addr(direccionDIEGO.ip));

	pthread_t hiloConsola = crearHilo(&consola, socketSAFA);
	pthread_t hiloEscuchadorSAFA = crearHilo(&escuchar, socketSAFA);
	pthread_t hiloEscuchadorFM9= crearHilo(&escuchar, socketFM9);
	pthread_t hiloEscuchadorDIEGO = crearHilo(&escuchar, socketDIEGO);

	esperarHilo(hiloConsola);
	esperarHilo(hiloEscuchadorSAFA);
	esperarHilo(hiloEscuchadorFM9);
	esperarHilo(hiloEscuchadorDIEGO);
	return 0;
}
