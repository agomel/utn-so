#include "CPU.h"

void escuchar(int socketSAFA){
	int a = 1;
	int tamanioPathEscriptorio;
	int tamanioBuffer;
	void* buffer;
	int desplazamiento;

	while(a){
		DTB* dtbRecibido;
		char header = deserializarChar(socketSAFA);
		int mensajeEntendido = 0;
		log_info(logger, "Iniciando retardo de %d........",retardo);
		usleep(retardo*1000);//Son milisegundos
		log_info(logger, "Finalizando retardo");
		sentencias = 0;
			switch(header){
				case ENVIAR_DTB:{
					dtbRecibido = deserializarDTB(socketSAFA);
					if(dtbRecibido->flag == 0){
						tratarDummy(dtbRecibido);
					}else{
						log_info(logger, "Recibi DTB NO Dummy");
						int continuar = 1;
						if(dtbRecibido->quantum != -1 && continuar){
							while(dtbRecibido->quantum != 0 && continuar){
								log_info(logger, "Tiene quantum el DTB");
								continuar = entenderLinea(dtbRecibido, mensajeEntendido, 0);
							}if(dtbRecibido->quantum == 0 && continuar){
								log_info(logger, "Termino quantum");
								serializarYEnviarDTB(socketSAFA, *dtbRecibido, logger, TERMINO_QUANTUM);
								enviarYSerializarIntSinHeader(socketSAFA, sentencias);
							}
						}else{
							int continuar2 = 1;
							while(continuar2){
								continuar2 = entenderLinea(dtbRecibido, mensajeEntendido, 1);
							}
						}
						log_info(logger, "Terminada operacion");
					}
					freeDTB(dtbRecibido);
					break;
				}
				case FINALIZARME:
					exit(1);
					break;
				default:
					log_error(logger, "Header desconocido %c del SAFA", header);
					break;
		}
	}
}

void despedida(){
	log_info(logger, "chauuuuu :)");
	enviarYSerializarCharSinHeader(socketSAFA, CHAUCHI_CPU);
	sleep(2);
	raise(SIGTERM);
}
int main(void) {
	signal(SIGINT, despedida);
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
