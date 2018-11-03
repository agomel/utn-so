
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
int sentencias;

char entendiendoLinea(char* lineaEjecutando, DTB* dtbRecibido){
	//Devuelve 'b' si lo llama al diego para que el safa lo bloquee
	//sino devuelve 's' para que siga ejecutando el escriptorio
	//'a' si hay que abortar el G.DT
	if(string_starts_with(lineaEjecutando, "abrir")){
		log_info(logger, "Ejecutando instruccion abrir");
		char* pathRecibido = asignarMemoria(strlen(lineaEjecutando)-5);
		pathRecibido = string_substring_from(lineaEjecutando, 6);
		//Corrobora si ya esta abierto
		if(listaContiene(dtbRecibido->listaDeArchivos, pathRecibido)){
			return 's';
		}else {
			//Bloquear dtb
			log_info(logger, "Bloquear DTB");
			serializarYEnviarDTB(socketSAFA, *dtbRecibido, logger, BLOQUEAR_DTB);
			freeDTB(dtbRecibido);
			enviarYSerializarIntSinHeader(socketSAFA, sentencias);
			char continuar = deserializarChar(socketSAFA);
			//continuar
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

	}else if(string_starts_with(lineaEjecutando, "concentrar")){
		log_info(logger, "Ejecutando instruccion concentrar");
		return 's';

	}else if(string_starts_with(lineaEjecutando, "asignar")){
		//Asignar
		log_info(logger, "Ejecutando instruccion asignar");
		char* parametros = string_substring_from(lineaEjecutando, 8);
		char** pathYNumeroLinea= string_n_split(parametros, 3, " ");
		char* path = pathYNumeroLinea[0];
		int numeroDeLinea = pathYNumeroLinea[1];
		char* datos = pathYNumeroLinea[2];
		if(listaContiene(dtbRecibido->listaDeArchivos, path)){
			//Esta abierto
			log_info(logger, "Ejecutando instruccion asignar");

			void* buffer = asignarMemoria(sizeof(char) + sizeof(int)*4 + (strlen(path)+1) + strlen(datos) + 1);
			int desplazamiento = 0;

			concatenarChar(buffer, &desplazamiento, ASIGNAR_DATOS);
			concatenarInt(buffer, &desplazamiento, dtbRecibido->id);
			concatenarString(buffer, &desplazamiento, path);
			concatenarInt(buffer, &desplazamiento, numeroDeLinea);
			concatenarString(buffer, &desplazamiento, datos);

			enviarMensaje(socketFM9, buffer, desplazamiento);

			free(buffer);

			return 'b';

		}else{
			//No esta abierto ese archivo
			return 'a';
		}
	}else if(string_starts_with(lineaEjecutando, "wait")){
		log_info(logger, "Ejecutando instruccion wait");
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

	}else if(string_starts_with(lineaEjecutando, "signal")){
		log_info(logger, "Ejecutando instruccion signal");
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

	}else if(string_starts_with(lineaEjecutando, "flush")){
		log_info(logger, "Ejecutando instruccion flush");
		char* pathRecibido = asignarMemoria(strlen(lineaEjecutando)-5);
		pathRecibido = string_substring_from(lineaEjecutando, 6);
		if(listaContiene(dtbRecibido->listaDeArchivos, pathRecibido)){
			//Bloquear dtb
			log_info(logger, "Bloquear DTB");
			serializarYEnviarDTB(socketSAFA, *dtbRecibido, logger, BLOQUEAR_DTB);
			freeDTB(dtbRecibido);
			enviarYSerializarIntSinHeader(socketSAFA, sentencias);
			char continuar = deserializarChar(socketSAFA);
			//continuar

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

	}else if(string_starts_with(lineaEjecutando, "close")){
		log_info(logger, "Ejecutando instruccion close");
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

					bool coincideNombre(char* pathAComparar){
							if(strcmp(pathAComparar, pathRecibido) == 0){
								return true;
							}
							return false;
						}
					list_remove_and_destroy_by_condition(dtbRecibido->listaDeArchivos, coincideNombre, free);
					free(pathRecibido);
					return 'b';
				}else{
					//No esta abierto ese archivo
					free(pathRecibido);
					return 'a';
				}
	}else if(string_starts_with(lineaEjecutando, "crear")){
		log_info(logger, "Ejecutando instruccion crear");

		//Bloquear dtb
		log_info(logger, "Bloquear DTB");
		serializarYEnviarDTB(socketSAFA, *dtbRecibido, logger, BLOQUEAR_DTB);
		freeDTB(dtbRecibido);
		enviarYSerializarIntSinHeader(socketSAFA, sentencias);
		char continuar = deserializarChar(socketSAFA);
		//continuar

		char* parametros = string_substring_from(lineaEjecutando, 6);
		char** pathYCantLineas = string_n_split(parametros, 2, ' ');
		char* path = pathYCantLineas[0];
		int cantidadDeLineas = pathYCantLineas[1];
		enviarySerializarPathyTamanioArchivo(socketDIEGO, path, cantidadDeLineas);
		return 'b';
	}else if(string_starts_with(lineaEjecutando, "borrar")){
		log_info(logger, "Ejecutando instruccion borrar");

		//Bloquear dtb
		log_info(logger, "Bloquear DTB");
		serializarYEnviarDTB(socketSAFA, *dtbRecibido, logger, BLOQUEAR_DTB);
		freeDTB(dtbRecibido);
		enviarYSerializarIntSinHeader(socketSAFA, sentencias);
		char continuar = deserializarChar(socketSAFA);
		//continuar

		char* pathRecibido = asignarMemoria(strlen(lineaEjecutando)-5);
		pathRecibido = string_substring_from(lineaEjecutando, 6);
		enviarYSerializarString(socketDIEGO, pathRecibido, BORRAR_ARCHIVO);
		free(pathRecibido);
		return 'b';
	}else{
		return 'a';
	}
}

void pedirCosasDelFM9(DTB* dtbRecibido){
	void* buffer;
	int desplazamiento = 0;
	int tamanioBuffer = sizeof(char) + sizeof(int) + strlen(dtbRecibido->escriptorio) + 1 + sizeof(int)*2;
	buffer = asignarMemoria(tamanioBuffer);

	concatenarChar(buffer, &desplazamiento, TRAER_LINEA_ESCRIPTORIO);
	concatenarInt(buffer, &desplazamiento, dtbRecibido->id);
	concatenarString(buffer, &desplazamiento, dtbRecibido->escriptorio);
	concatenarInt(buffer, &desplazamiento, dtbRecibido->programCounter);
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
		sentencias = 0;
			switch(header){
				case ENVIAR_DTB:{
					log_info(logger, "Recibiendo un dtb");
					dtbRecibido = deserializarDTB(socketSAFA);
					if(dtbRecibido->flag == 0){
						log_info(logger, "Recibi DTB Dummy");
						//Es el dummy
						serializarYEnviarDTB(socketSAFA, *dtbRecibido, logger, DUMMY);
						log_debug(logger, "esperando que SAFA desbloquee para continuar");
						deserializarChar(socketSAFA);
						log_debug(logger, "continuo ejecucion");

						int tamanioPathEscriptorio = strlen(dtbRecibido->escriptorio) + 1;
						int tamanioBuffer = sizeof(char) + tamanioPathEscriptorio + sizeof(int)*3;
						void* buffer = asignarMemoria(tamanioBuffer);
						int desplazamiento = 0;

						concatenarChar(buffer, &desplazamiento, CARGAR_ESCRIPTORIO_EN_MEMORIA);
						concatenarInt(buffer, &desplazamiento, dtbRecibido->id);
						concatenarString(buffer, &desplazamiento, dtbRecibido->escriptorio);
						concatenarInt(buffer, &desplazamiento, 0);
						enviarMensaje(socketDIEGO, buffer, tamanioBuffer);

						free(buffer);
						log_info(logger, "Enviando a SAFA que desbloquee dummy");
						serializarYEnviarDTB(socketSAFA, *dtbRecibido, logger, DESBLOQUEAR_DTB);
						freeDTB(dtbRecibido);
						enviarYSerializarIntSinHeader(socketSAFA, 1); //Ejecuta una sentencia
					}else{
						log_info(logger, "Recibi DTB NO Dummy");
						char* lineaAEjecutar;
						//No es el dummy
						int continuar = 1;
						if(dtbRecibido->quantum != -1 && continuar){
							while(dtbRecibido->quantum != 0 && continuar){

								log_info(logger, "Tiene quantum el DTB");
								pedirCosasDelFM9(dtbRecibido);
								lineaAEjecutar = deserializarString(socketFM9);
								if(lineaAEjecutar[0] == FIN_ARCHIVO){
									//Fin de archivo
									log_info(logger, "Pasar DTB a EXIT");
									serializarYEnviarDTB(socketSAFA, *dtbRecibido, logger, PASAR_A_EXIT);
									enviarYSerializarIntSinHeader(socketSAFA, sentencias);
									freeDTB(dtbRecibido);
									continuar = 0;
								}else if(lineaAEjecutar[0] == ERROR_O_ACCESO_INVALIDO){
									//Hubo error en FM9
									dtbRecibido->quantum--;
									sentencias++;
									log_info(logger, "Pasar DTB a EXIT");
									serializarYEnviarDTB(socketSAFA, *dtbRecibido, logger, PASAR_A_EXIT);
									enviarYSerializarIntSinHeader(socketSAFA, sentencias);
									freeDTB(dtbRecibido);
									continuar = 0;
								}else if(lineaAEjecutar[0] != '#'){
									sentencias++;
									dtbRecibido->programCounter++;
									mensajeEntendido = entendiendoLinea(lineaAEjecutar, dtbRecibido);
									if(mensajeEntendido == 'b'){
										continuar = 0;
									}else if(mensajeEntendido == 'a'){
										log_info(logger, "Pasar DTB a EXIT");
										serializarYEnviarDTB(socketSAFA, *dtbRecibido, logger, PASAR_A_EXIT);
										freeDTB(dtbRecibido);
										enviarYSerializarIntSinHeader(socketSAFA, sentencias);
										continuar = 0;
									}
								}
									dtbRecibido->programCounter++;
									dtbRecibido->quantum--;
									sentencias++;
							log_info(logger, "Ejecutando una linea del escriptorio");
							}if(dtbRecibido->quantum == 0){
								log_info(logger, "Termino quantum");
								serializarYEnviarDTB(socketSAFA, *dtbRecibido, logger, TERMINO_QUANTUM);
								enviarYSerializarIntSinHeader(socketSAFA, sentencias);
								freeDTB(dtbRecibido);
							}
						}else{
							pedirCosasDelFM9(dtbRecibido);
							lineaAEjecutar = deserializarString(socketFM9);
							int b = 1;
							while(b){
								if(lineaAEjecutar[0] == ERROR_O_ACCESO_INVALIDO || lineaAEjecutar[0] == FIN_ARCHIVO){
									log_info(logger, "finalizando archivo");
									//Fin de archivo o hubo un error
									sentencias++;
									serializarYEnviarDTB(socketSAFA, *dtbRecibido, logger, PASAR_A_EXIT);
									enviarYSerializarIntSinHeader(socketSAFA, sentencias);
									b = 0;
								}else if(lineaAEjecutar[0] != '#'){
									mensajeEntendido = entendiendoLinea(lineaAEjecutar, dtbRecibido);
									sentencias++;
									if(mensajeEntendido == 'b'){
										dtbRecibido->programCounter++;
										b = 0;
									}else if(mensajeEntendido == 'a'){
										log_info(logger, "Pasar DTB a EXIT");
										serializarYEnviarDTB(socketSAFA, *dtbRecibido, logger, PASAR_A_EXIT);
										freeDTB(dtbRecibido);
										enviarYSerializarIntSinHeader(socketSAFA, sentencias);
										b = 0;
									}
								}
								if(b == 1){
									dtbRecibido->programCounter++;
									pedirCosasDelFM9(dtbRecibido);
									lineaAEjecutar = deserializarString(socketFM9);
								}
							}
						}
					}
					break;
				}
				default:
					log_error(logger, "Header desconocido %c del SAFA", header);
					break;
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
