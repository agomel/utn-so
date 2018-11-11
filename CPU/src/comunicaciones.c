#include "comunicaciones.h"


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

void tratarDummy(DTB* dtbRecibido){
	log_info(logger, "Recibi DTB Dummy");
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
	enviarYSerializarIntSinHeader(socketSAFA, 1); //Ejecuta una sentencia
}

int entenderLinea(char* lineaAEjecutar, DTB* dtbRecibido, char mensajeEntendido, int fifo){
	int continuar = 0;
	pedirCosasDelFM9(dtbRecibido);
	lineaAEjecutar = deserializarString(socketFM9);
	if(lineaAEjecutar[0] == FIN_ARCHIVO){
		//Fin de archivo
		log_info(logger, "Pasar DTB a EXIT");
		serializarYEnviarDTB(socketSAFA, *dtbRecibido, logger, PASAR_A_EXIT);
		enviarYSerializarIntSinHeader(socketSAFA, sentencias);
	}else if(lineaAEjecutar[0] == ERROR_O_ACCESO_INVALIDO){
		//Hubo error en FM9
		if(!fifo){
			dtbRecibido->quantum--;
			sentencias++;
		}
		log_info(logger, "Pasar DTB a EXIT");
		serializarYEnviarDTB(socketSAFA, *dtbRecibido, logger, PASAR_A_EXIT);
		enviarYSerializarIntSinHeader(socketSAFA, sentencias);
	}else if(lineaAEjecutar[0] != '#'){
		sentencias++;
		mensajeEntendido = entendiendoLinea(lineaAEjecutar, dtbRecibido);
		if(mensajeEntendido == 'b'){
			//NADA
		}else if(mensajeEntendido == 'a'){
		log_info(logger, "Pasar DTB a EXIT");
		serializarYEnviarDTB(socketSAFA, *dtbRecibido, logger, PASAR_A_EXIT);
		enviarYSerializarIntSinHeader(socketSAFA, sentencias);
		}else{
			continuar = 1;
		}
	}
	dtbRecibido->programCounter++;
	if(!fifo){
		dtbRecibido->quantum--;
	}
	log_info(logger, "Ejecutando una linea del escriptorio");
	return continuar;
}

