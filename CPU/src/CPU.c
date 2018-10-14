
#include <biblioteca/socket.h>
#include <biblioteca/hilos.h>
#include <biblioteca/utilidades.h>
#include <biblioteca/dtb.h>
#include <biblioteca/logger.h>

int socketDIEGO;
int socketFM9;
int socketSAFA;
t_log* logger;

char entendiendoLinea(char* lineaEjecutando, DTB dtbRecibido){ //Devuelve 'b' si lo llama al diego para que el safa lo bloquee, sino devuelve 's' para que siga ejecutando el escriptorio
	if(string_starts_with(lineaEjecutando, "abrir")){
		log_info(logger, "Ejecutando instruccion abrir");
		char * pathRecibido = string_substring_from(lineaEjecutando, 4);
		//Corroborar si ya esta abierto
		if(dictionary_has_key(dtbRecibido.direccionesArchivos, pathRecibido)){
			return 's';
		}else {
			int tamanioPathEscriptorioACargar;
			int tamanioBuffer2;
			int desplazamiento = 0;
			void* buffer2;
			tamanioPathEscriptorioACargar = strlen(pathRecibido) + 1;
			tamanioBuffer2 = sizeof(char) + tamanioPathEscriptorioACargar + sizeof(int)*2;
			buffer2 = asignarMemoria(tamanioBuffer2);

			concatenarChar(buffer2, &desplazamiento, CARGAR_ESCRIPTORIO_EN_MEMORIA);
			concatenarString(buffer2, &desplazamiento, dtbRecibido.escriptorio);
			concatenarInt(buffer2, &desplazamiento, dtbRecibido.id);
			enviarMensaje(socketDIEGO, buffer2, tamanioBuffer2);
			free(buffer2);

			return 'b';
		}
	}else if(string_starts_with(lineaEjecutando, "concentrar")){
		log_info(logger, "Ejecutando instruccion concentrar");
		return 's';
	}else if(string_starts_with(lineaEjecutando, "asignar")){
		//Asignar
		log_info(logger, "Ejecutando instruccion asignar");
	}else if(string_starts_with(lineaEjecutando, "wait")){
		//Wait
		log_info(logger, "Ejecutando instruccion wait");
	}else if(string_starts_with(lineaEjecutando, "signal")){
		//Signal
		log_info(logger, "Ejecutando instruccion signal");
	}else if(string_starts_with(lineaEjecutando, "flush")){
		//Flush
		log_info(logger, "Ejecutando instruccion flush");
	}else if(string_starts_with(lineaEjecutando, "close")){
		//Close
		log_info(logger, "Ejecutando instruccion close");
	}else if(string_starts_with(lineaEjecutando, "crear")){
		//Crear
		log_info(logger, "Ejecutando instruccion crear");
	}else if(string_starts_with(lineaEjecutando, "borrar")){
		//Borrar
		log_info(logger, "Ejecutando instruccion borrar");
	}
}

void pedirCosasDelFM9(DTB dtbRecibido){
	void* buffer;
	int desplazamiento = 0;
	t_list* listaDeDirecciones = list_create();
	listaDeDirecciones = dictionary_get(dtbRecibido.direccionesArchivos, dtbRecibido.escriptorio);
	int tamanioBuffer = sizeof(char) + sizeof(int)*2 + sizeof(int)*(listaDeDirecciones->elements_count);
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
					log_debug(logger, "Recibiendo un dtb");
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
						//enviarMensaje(socketSAFA, DESBLOQUEAR_DTB, sizeof(char));
						//serializarYEnviarDTB(socketSAFA, dtbRecibido, logger);
						//freeDTB(&dtbRecibido);
					}else{
						char* lineaAEjecutar;
						//No es el dummy
						if(dtbRecibido.quantum > 0){
							while(dtbRecibido.quantum > 0){
								//MensajeNano: Preguntarle al safa si quiere que deje de ejecutar
								pedirCosasDelFM9(dtbRecibido);
								lineaAEjecutar = deserializarString(socketFM9);
								if(lineaAEjecutar[0] == 'FIN_ARCHIVO'){
									//Fin de archivo
									enviarMensaje(socketSAFA, PASAR_A_EXIT, sizeof(char));
									serializarYEnviarDTB(socketSAFA, dtbRecibido, logger);
									break;
								}else if(lineaAEjecutar[0] == 'ERROR_O_ACCESO_INVALIDO'){
									//Hubo error en FM9
									dtbRecibido.quantum--;
									enviarMensaje(socketSAFA, PASAR_A_EXIT, sizeof(char));
									serializarYEnviarDTB(socketSAFA, dtbRecibido, logger);
									break;
								}else if(lineaAEjecutar[0] != '#'){
									mensajeEntendido = entendiendoLinea(lineaAEjecutar, dtbRecibido);
									if(mensajeEntendido == 'b'){
										dtbRecibido.programCounter++;
										enviarMensaje(socketSAFA, BLOQUEAR_DTB, sizeof(char));
										serializarYEnviarDTB(socketSAFA, dtbRecibido, logger);
										freeDTB(*dtbRecibido);
										break;
									}
								}
									dtbRecibido.programCounter++;
									dtbRecibido.quantum--;
							log_info(logger, "Ejecutando una linea del escriptorio");
							}if(dtbRecibido.quantum == 0){
								enviarMensaje(socketSAFA, TERMINO_QUANTUM, sizeof(char));
								serializarYEnviarDTB(socketSAFA, dtbRecibido, logger);
							}
						}else{
							//MensajeNano: Preguntarle al safa si quiere que deje de ejecutar
							pedirCosasDelFM9(dtbRecibido);
							lineaAEjecutar = deserializarString(socketFM9);
							while(1){
								if(lineaAEjecutar[0] == ERROR_O_ACCESO_INVALIDO || lineaAEjecutar[0] == FIN_ARCHIVO){
									//Fin de archivo o hubo un error
									enviarMensaje(socketSAFA, PASAR_A_EXIT, sizeof(char));
									serializarYEnviarDTB(socketSAFA, dtbRecibido, logger);
									break;
								}else if(lineaAEjecutar[0] != '#'){
									mensajeEntendido = entendiendoLinea(lineaAEjecutar, dtbRecibido);
									if(mensajeEntendido == 'b'){
										dtbRecibido.programCounter++;
										enviarMensaje(socketSAFA, BLOQUEAR_DTB, sizeof(char));
										serializarYEnviarDTB(socketSAFA, dtbRecibido, logger);
									}
								}
								dtbRecibido.programCounter++;
								//MensajeNano: Preguntarle al safa si quiere que deje de ejecutar
								pedirCosasDelFM9(dtbRecibido);
								lineaAEjecutar = deserializarString(socketFM9);
							}
						}
					}
					break;
				default:
					log_error(logger, "Header desconocido");
		}
	}
}

int main(void) {
	logger = crearLogger(ARCHIVO_LOG, "CPU");

	direccionServidor direccionSAFA = levantarDeConfiguracion("IP_SAFA", "PUERTO_SAFA", ARCHIVO_CONFIGURACION);
	socketSAFA = conectarConServidor(direccionSAFA.puerto, inet_addr(direccionSAFA.ip));
	handshake(socketSAFA, CPU);

	direccionServidor direccionFM9 = levantarDeConfiguracion("IP_FM9", "PUERTO_FM9", ARCHIVO_CONFIGURACION);
	socketFM9 = conectarConServidor(direccionFM9.puerto, inet_addr(direccionFM9.ip));
	handshake(socketFM9, CPU);

	direccionServidor direccionDIEGO = levantarDeConfiguracion("IP_DIEGO", "PUERTO_DIEGO", ARCHIVO_CONFIGURACION);
	socketDIEGO= conectarConServidor(direccionDIEGO.puerto, inet_addr(direccionDIEGO.ip));

	escuchar(socketSAFA);
	return 0;
}
