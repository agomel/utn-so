
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

char entendiendoLinea(char* lineaEjecutando){ //Devuelve 'b' si lo llama al diego para que el safa lo bloquee, sino devuelve 's' para que siga ejecutando el escriptorio
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
	t_list* listaDeDirecciones = list_create();
	listaDeDirecciones = dictionary_get(dtbRecibido.direccionesArchivos, dtbRecibido.escriptorio);
	u_int32_t tamanioBuffer = sizeof(char) + sizeof(u_int32_t)*2 + sizeof(u_int32_t)*(listaDeDirecciones->elements_count);
	buffer = asignarMemoria(tamanioBuffer);

	concatenarChar(buffer, &desplazamiento, TRAER_LINEA_ESCRIPTORIO);
	concatenarInt(buffer, &desplazamiento, dtbRecibido.programCounter);
	concatenarInt(buffer, &desplazamiento, listaDeDirecciones->elements_count);
	concatenarListaInt(buffer, &desplazamiento, listaDeDirecciones);
	enviarMensaje(socketFM9, buffer, tamanioBuffer);
	free(buffer);
}
void escuchar(int servidor){
	int a = 1;
	int tamanioPathEscriptorio;
	int tamanioBuffer;
	void* buffer;
	int desplazamiento;

	while(a){
		DTB dtbRecibido;
		char header = deserializarChar(servidor);
		char mensajeEntendido = 's';
			switch(header){
				case ENVIAR_DTB:
					printf("llego un dtb\n");
					dtbRecibido = deserializarDTB(servidor);
					if(dtbRecibido.flag == 0){
						//Es el dummy
						tamanioPathEscriptorio = strlen(dtbRecibido.escriptorio) + 1;
						tamanioBuffer = sizeof(char) + tamanioPathEscriptorio + sizeof(int)*2;
						buffer = asignarMemoria(tamanioBuffer);
						desplazamiento = 0;

						concatenarChar(buffer, &desplazamiento, CARGAR_ESCRIPTORIO_EN_MEMORIA);
						concatenarString(buffer, &desplazamiento, dtbRecibido.escriptorio);
						concatenarInt(buffer, &desplazamiento, dtbRecibido.id);

						enviarMensaje(socketDIEGO, buffer, tamanioBuffer);
						free(buffer);
						desplazamiento = 0;
						enviarMensaje(socketSAFA, DESBLOQUEAR_DTB, sizeof(char));
						serializarYEnviarDTB(socketSAFA, dtbRecibido);
						freeDTB(&dtbRecibido);
					}else{
						char* lineaAEjecutar;
						//No es el dummy
						if(dtbRecibido.quantum > 0){
							while(dtbRecibido.quantum > 0){
								pedirCosasDelFM9(dtbRecibido);
								lineaAEjecutar = deserializarString(socketFM9);
								if(lineaAEjecutar[0] == '@'){
									//Fin de archivo
									enviarMensaje(socketSAFA, PASAR_A_EXIT, sizeof(char));
									serializarYEnviarDTB(socketSAFA, dtbRecibido);
									break;
								}else if(lineaAEjecutar[0] == '!'){
									//Hubo error en FM9
									dtbRecibido.quantum--;
									enviarMensaje(socketSAFA, PASAR_A_EXIT, sizeof(char));
									serializarYEnviarDTB(socketSAFA, dtbRecibido);
									break;
								}else if(lineaAEjecutar[0] != '#'){
									mensajeEntendido = entendiendoLinea(lineaAEjecutar);
									if(mensajeEntendido == 'b'){
										dtbRecibido.programCounter++;
										enviarMensaje(socketSAFA, BLOQUEAR_DTB, sizeof(char));
										serializarYEnviarDTB(socketSAFA, dtbRecibido);
									}
								}
									dtbRecibido.programCounter++;
									dtbRecibido.quantum--;
							//MensajeNano: Enviarle al Safa BLOQUEAR_DTB con el dtbrecibido si interviene el DAM en la ejecucion
							printf("Ejecutando una linea del escriptorio");
							}if(dtbRecibido.quantum == 0){
								enviarMensaje(socketSAFA, TERMINO_QUANTUM, sizeof(char));
								serializarYEnviarDTB(socketSAFA, dtbRecibido);
							}
						}else{
							pedirCosasDelFM9(dtbRecibido);
							lineaAEjecutar = deserializarString(socketFM9);
							while(1){
								if(lineaAEjecutar[0] == '@'|| lineaAEjecutar[0] == '!'){
									//Fin de archivo o hubo un error
									enviarMensaje(socketSAFA, PASAR_A_EXIT, sizeof(char));
									serializarYEnviarDTB(socketSAFA, dtbRecibido);
									break;
								}else if(lineaAEjecutar[0] != '#'){
									mensajeEntendido = entendiendoLinea(lineaAEjecutar);
									if(mensajeEntendido == 'b'){
										dtbRecibido.programCounter++;
										enviarMensaje(socketSAFA, BLOQUEAR_DTB, sizeof(char));
										serializarYEnviarDTB(socketSAFA, dtbRecibido);
									}
								}
								dtbRecibido.programCounter++;
								pedirCosasDelFM9(dtbRecibido);
								lineaAEjecutar = deserializarString(socketFM9);
							}
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
