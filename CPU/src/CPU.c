
#include <biblioteca/socket.h>
#include <biblioteca/hilos.h>
#include <biblioteca/utilidades.h>
#include <biblioteca/dtb.h>
#include <biblioteca/logger.h>
#include <unistd.h>

int socketDIEGO;
int socketFM9;
int socketSAFA;
t_log* logger;
int retardo;

char entendiendoLinea(char* lineaEjecutando, DTB* dtbRecibido){
	//Devuelve 'b' si lo llama al diego para que el safa lo bloquee
	//sino devuelve 's' para que siga ejecutando el escriptorio
	//'a' si hay que abortar el G.DT
	if(string_starts_with(lineaEjecutando, "abrir")){
		log_info(logger, "Ejecutando instruccion abrir");
		char* pathRecibido = asignarMemoria(strlen(lineaEjecutando)-4);
		pathRecibido = string_substring_from(lineaEjecutando, 5);
		//Corrobora si ya esta abierto
		if(dictionary_has_key(dtbRecibido->direccionesArchivos, pathRecibido)){
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
			concatenarString(buffer2, &desplazamiento, dtbRecibido->escriptorio);
			concatenarInt(buffer2, &desplazamiento, dtbRecibido->id);
			enviarMensaje(socketDIEGO, buffer2, tamanioBuffer2);
			free(buffer2);
			free(pathRecibido);
			return 'b';
		}

	}else if(string_starts_with(lineaEjecutando, "concentrar")){
		log_info(logger, "Ejecutando instruccion concentrar");
		return 's';

	}else if(string_starts_with(lineaEjecutando, "asignar")){
		//Asignar
		log_info(logger, "Ejecutando instruccion asignar");

	}else if(string_starts_with(lineaEjecutando, "wait")){
		log_info(logger, "Ejecutando instruccion wait");
		char* recursoRecibido = asignarMemoria(strlen(lineaEjecutando)-3);
		recursoRecibido = string_substring_from(lineaEjecutando, 4);
		enviarYSerializarString(socketSAFA, recursoRecibido, RETENCION_DE_RECURSO);
		char seguirConEjecucion = deserializarChar(socketSAFA);
		free(recursoRecibido);
		if(seguirConEjecucion == 'LIBERAR_DTB_DE_EJECUCION'){
			return 'b';
		}else{
			return 's';
		}

	}else if(string_starts_with(lineaEjecutando, "signal")){
		log_info(logger, "Ejecutando instruccion signal");
		char* recursoRecibido = asignarMemoria(strlen(lineaEjecutando)-5);
		recursoRecibido = string_substring_from(lineaEjecutando, 6);
		enviarYSerializarString(socketSAFA, recursoRecibido, LIBERAR_RECURSO);
		char seguirConEjecucion = deserializarChar(socketSAFA);
		free(recursoRecibido);
		if(seguirConEjecucion == 'LIBERAR_DTB_DE_EJECUCION'){
			return 'b';
		}else{
			return 's';
		}

	}else if(string_starts_with(lineaEjecutando, "flush")){
		log_info(logger, "Ejecutando instruccion flush");
		char* pathRecibido = asignarMemoria(strlen(lineaEjecutando)-4);
		pathRecibido = string_substring_from(lineaEjecutando, 5);
		if(dictionary_has_key(dtbRecibido->direccionesArchivos, pathRecibido)){
			//Esta abierto
			void* buffer;
			int desplazamiento = 0;
			int tamanioBuffer = sizeof(char) + (strlen(pathRecibido)+1) + sizeof(int)*2 + obtenerTamanioDiccionario(dtbRecibido->direccionesArchivos);
			buffer = asignarMemoria(tamanioBuffer);

			concatenarChar(buffer, &desplazamiento, GUARDAR_ESCRIPTORIO);
			concatenarString(buffer, &desplazamiento, pathRecibido);
			concatenarDiccionario(buffer, &desplazamiento, dtbRecibido->direccionesArchivos);
			enviarMensaje(socketDIEGO, buffer, tamanioBuffer);
			free(buffer);
			free(pathRecibido);
			return 'b';
		}else{
			//No esta abierto ese archivo
			return 'a';
		}

	}else if(string_starts_with(lineaEjecutando, "close")){
		//Close
		log_info(logger, "Ejecutando instruccion close");
		char* pathRecibido = asignarMemoria(strlen(lineaEjecutando)-4);
				pathRecibido = string_substring_from(lineaEjecutando, 5);
				if(dictionary_has_key(dtbRecibido->direccionesArchivos, pathRecibido)){
					//Esta abierto
					void* buffer;
					int desplazamiento = 0;
					int tamanioBuffer = sizeof(char) + (strlen(pathRecibido)+1) + sizeof(int)*2 + obtenerTamanioDiccionario(dtbRecibido->direccionesArchivos);
					buffer = asignarMemoria(tamanioBuffer);

					concatenarChar(buffer, &desplazamiento, LIBERAR_MEMORIA);
					concatenarString(buffer, &desplazamiento, pathRecibido);
					concatenarDiccionario(buffer, &desplazamiento, dtbRecibido->direccionesArchivos);
					enviarMensaje(socketFM9, buffer, tamanioBuffer);
					free(buffer);
					for (int i = 0; i < dtbRecibido->direccionesArchivos; ++i) {
						//MensajeNano: Ver que hay que remover en el diccionario
						//dictionary_remove_and_destroy(dtbRecibido->direccionesArchivos, dtbRecibido->escriptorio, )
					}
					free(pathRecibido);
					return 'b';
				}else{
					//No esta abierto ese archivo
					return 'a';
				}
	}else if(string_starts_with(lineaEjecutando, "crear")){
		//Crear
		log_info(logger, "Ejecutando instruccion crear");
	}else if(string_starts_with(lineaEjecutando, "borrar")){
		log_info(logger, "Ejecutando instruccion borrar");
		char* pathRecibido = asignarMemoria(strlen(lineaEjecutando)-4);
		pathRecibido = string_substring_from(lineaEjecutando, 5);
		enviarYSerializarString(socketDIEGO, pathRecibido, BORRAR_DATOS);
		free(pathRecibido);
		return 'b';
	}else{
		return 'a';
	}
}

void pedirCosasDelFM9(DTB* dtbRecibido){
	void* buffer;
	int desplazamiento = 0;
	t_list* listaDeDirecciones = list_create();
	listaDeDirecciones = dictionary_get(dtbRecibido->direccionesArchivos, dtbRecibido->escriptorio);
	int tamanioBuffer = sizeof(char) + sizeof(int)*2 + sizeof(int)*(listaDeDirecciones->elements_count);
	buffer = asignarMemoria(tamanioBuffer);

	concatenarChar(buffer, &desplazamiento, TRAER_LINEA_ESCRIPTORIO);
	concatenarInt(buffer, &desplazamiento, dtbRecibido->programCounter);
	concatenarListaInt(buffer, &desplazamiento, listaDeDirecciones);
	enviarMensaje(socketFM9, buffer, tamanioBuffer);
	free(buffer);
}
void escuchar(int socketSAFA){//MensajeNano: Verificar los punteros de DTB
	int a = 1;
	int tamanioPathEscriptorio;
	int tamanioBuffer;
	void* buffer;
	int desplazamiento;

	while(a){
		DTB* dtbRecibido;
		char header = deserializarChar(socketSAFA);
		char mensajeEntendido = 's';
		log_info(logger, "Iniciando retardo de %d........",retardo);
		usleep(retardo*1000);//Son milisegundos
		log_info(logger, "Finalizando retardo");
			switch(header){
				case ENVIAR_DTB:
					log_info(logger, "Recibiendo un dtb");
					dtbRecibido = deserializarDTB(socketSAFA);
					if(dtbRecibido->flag == 0){
						log_info(logger, "Recibi DTB Dummy");
						//Es el dummy
						int tamanioPathEscriptorio = strlen(dtbRecibido->escriptorio) + 1;
						int tamanioBuffer = sizeof(char) + tamanioPathEscriptorio + sizeof(int)*2;
						void* buffer = asignarMemoria(tamanioBuffer);
						int desplazamiento = 0;

						concatenarChar(buffer, &desplazamiento, CARGAR_ESCRIPTORIO_EN_MEMORIA);
						concatenarInt(buffer, &desplazamiento, dtbRecibido->id);
						concatenarString(buffer, &desplazamiento, dtbRecibido->escriptorio);


						enviarMensaje(socketDIEGO, buffer, tamanioBuffer);
						free(buffer);
						log_info(logger, "Enviando a SAFA que desbloquee dummy");
						serializarYEnviarDTB(socketSAFA, *dtbRecibido, logger, DESBLOQUEAR_DTB);
						freeDTB(dtbRecibido);
					}else{
						log_info(logger, "Recibi DTB NO Dummy");
						char* lineaAEjecutar;
						//No es el dummy
						if(dtbRecibido->quantum != -1){
							while(dtbRecibido->quantum != 0){

								log_info(logger, "Tiene quantum el DTB");
								pedirCosasDelFM9(dtbRecibido);
								lineaAEjecutar = deserializarString(socketFM9);
								if(lineaAEjecutar[0] == 'FIN_ARCHIVO'){
									//Fin de archivo
									log_info(logger, "Pasar DTB a EXIT");
									serializarYEnviarDTB(socketSAFA, *dtbRecibido, logger, PASAR_A_EXIT);
									break;
								}else if(lineaAEjecutar[0] == 'ERROR_O_ACCESO_INVALIDO'){
									//Hubo error en FM9
									dtbRecibido->quantum--;
									log_info(logger, "Pasar DTB a EXIT");
									serializarYEnviarDTB(socketSAFA, *dtbRecibido, logger, PASAR_A_EXIT);
									break;
								}else if(lineaAEjecutar[0] != '#'){
									mensajeEntendido = entendiendoLinea(lineaAEjecutar, dtbRecibido);
									if(mensajeEntendido == 'b'){
										dtbRecibido->programCounter++;
										log_info(logger, "Bloquear DTB");
										serializarYEnviarDTB(socketSAFA, *dtbRecibido, logger, BLOQUEAR_DTB);
										freeDTB(dtbRecibido);
										break;
									}else if(mensajeEntendido == 'a'){
										log_info(logger, "Pasar DTB a EXIT");
										serializarYEnviarDTB(socketSAFA, *dtbRecibido, logger, PASAR_A_EXIT);
										freeDTB(dtbRecibido);
										break;
									}
								}
									dtbRecibido->programCounter++;
									dtbRecibido->quantum--;
							log_info(logger, "Ejecutando una linea del escriptorio");
							}if(dtbRecibido->quantum == 0){
								log_info(logger, "Termino quantum");
								serializarYEnviarDTB(socketSAFA, *dtbRecibido, logger, TERMINO_QUANTUM);
							}
						}else{
							pedirCosasDelFM9(dtbRecibido);
							lineaAEjecutar = deserializarString(socketFM9);
							while(1){
								if(lineaAEjecutar[0] == ERROR_O_ACCESO_INVALIDO || lineaAEjecutar[0] == FIN_ARCHIVO){
									//Fin de archivo o hubo un error
									serializarYEnviarDTB(socketSAFA, *dtbRecibido, logger, PASAR_A_EXIT);
									break;
								}else if(lineaAEjecutar[0] != '#'){
									mensajeEntendido = entendiendoLinea(lineaAEjecutar, dtbRecibido);
									if(mensajeEntendido == 'b'){
										dtbRecibido->programCounter++;
										log_info(logger, "Bloquear DTB");
										serializarYEnviarDTB(socketSAFA, *dtbRecibido, logger, BLOQUEAR_DTB);
									}else if(mensajeEntendido == 'a'){
										log_info(logger, "Pasar DTB a EXIT");
										serializarYEnviarDTB(socketSAFA, *dtbRecibido, logger, PASAR_A_EXIT);
										freeDTB(dtbRecibido);
										break;
									}
								}
								dtbRecibido->programCounter++;
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

	t_config* configuracion = config_create(ARCHIVO_CONFIGURACION);
	retardo = config_get_int_value(configuracion, "RETARDO");

	direccionServidor direccionSAFA = levantarDeConfiguracion("IP_SAFA", "PUERTO_SAFA", configuracion);
	socketSAFA = conectarConServidor(direccionSAFA.puerto, inet_addr(direccionSAFA.ip));
	handshake(socketSAFA, CPU);

	direccionServidor direccionFM9 = levantarDeConfiguracion("IP_FM9", "PUERTO_FM9", configuracion);
	socketFM9 = conectarConServidor(direccionFM9.puerto, inet_addr(direccionFM9.ip));
	handshake(socketFM9, CPU);

	direccionServidor direccionDIEGO = levantarDeConfiguracion("IP_DIEGO", "PUERTO_DIEGO", configuracion);
	socketDIEGO= conectarConServidor(direccionDIEGO.puerto, inet_addr(direccionDIEGO.ip));
	handshake(socketDIEGO, CPU);

	config_destroy(configuracion);

	escuchar(socketSAFA);
	return 0;
}
