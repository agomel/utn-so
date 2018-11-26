#include "plp.h"

void planificadorALargoPlazo() {
	int a = 1;
	while (a) {
		waitSem(&semCantidadEnNew);

		DTB* dtb = obtenerPrimerDTBEnNew();

		waitSem(&semDummy);

		cargarDummy(*dtb);

		signalSem(&cantidadTotalREADY);
	}
}

void cargarDummy(DTB dtb) {
	waitMutex(&mutexDummy);
	dtbDummy->flag = 0;
	dtbDummy->escriptorio = dtb.escriptorio;
	dtbDummy->id = dtb.id;
	dtbDummy->programCounter = 0;
	dtbDummy->listaDeArchivos = list_create();
	dtbDummy->estado = READY;
	dtbDummy->tamanioArchivosAbiertos = 9999;
	cambiarEstadoDummyCargandolo(dtbDummy);
	signalMutex(&mutexDummy);
}

void ponerProcesoEnNew(char* escriptorio) {
	DTB* dtb = asignarMemoria(sizeof(DTB));
	*dtb = crearDTB(escriptorio);
	agregarDTBALista(dtb);

	Historial* historialNEW = crearHistorial(dtb->id);
	agregarHistorialAListaNew(historialNEW);

	Historial* historialEXIT = crearHistorial(dtb->id);
	agregarHistorialAListaExit(historialEXIT);

	signalSem(&semCantidadEnNew);
}

void ponerEnReady(int idDTB) {
	DTB* dtb = obtenerDTBDeCola(idDTB);
	if(dtb->estado == MANDADO_A_DUMIZZAR){
		finalizarHistorialDeListaNew(idDTB);
	}
	cambiarEstado(idDTB, READY);
	signalSem(&cantidadTotalREADY);
}

void ponerEnReadyDTB(DTB* dtb) {
	if(dtb->estado == MANDADO_A_DUMIZZAR){
		finalizarHistorialDeListaNew(dtb->id);
	}
	cambiarEstadoGuardandoNuevoDTB(dtb, READY);
	signalSem(&cantidadTotalREADY);
}

void pasarDTBAExit(int idDTB){
	DTB* dtb = obtenerDTBDeCola(idDTB);
	if(!dtb){
		log_error(logger,"NO existe ese dtb");
	}else if(dtb->estado != EXIT){
		cambiarEstado(idDTB, EXIT);
		finalizarHistorialDeListaExit(idDTB);
		signalSem(&gradoMultiprogramacion);
	}
}
void pasarDTBAExitGuardandoNuevo(DTB* dtb) {
	if(dtb->estado != EXIT){
		cambiarEstadoGuardandoNuevoDTB(dtb, EXIT);
		finalizarHistorialDeListaExit(dtb->id);
		signalSem(&gradoMultiprogramacion);
	}
}

void manejarErrores(int idDTB, char* path, int error){
	pasarDTBAExit(idDTB);
	switch(error){
		case PATH_INEXISTENTE:
			log_error(logger, "path inexistente de archivo %s", path);
			break;

		case ESPACIO_INSUFICIENTE_EN_FM9:
			log_error(logger, "espacio insuficiente en FM9 para archivo %s", path);
			break;

		case EL_ARCHIVO_NO_SE_ENCUENTRA_ABIERTO:
			log_error(logger, "El archivo no se encuentra abierto %s", path);
			break;

		case FALLO_DE_SEGMENTO_MEMORIA:
			log_error(logger, "Fallo de segmento/memoria en path %s", path);
			break;

		case ESPACIO_INSUFICIENTE_EN_MDJ:
			log_error(logger, "Espacio insuficiente en MDJ para archivo %s", path);
			break;

		case EL_ARCHIVO_NO_EXISTE_EN_MDJ:
			log_error(logger, "El archivo no existe en MDJ, fue borrado previamente MDJ path: %s", path);
			break;

		case ARCHIVO_YA_EXISTENTE:
			log_error(logger, "Archivo ya existente: %s", path);
			break;

		default:
			log_error(logger, "No reconozco el error, pero te termino el dtb. Numero de error: %d", error);
	}
}

void verificarSiPasarAExit(int emisor, DTB* dtb){
	waitMutex(&mutexCpusAFinalizarDTBs);
	int tieneLaClave = dictionary_has_key(cpusAFinalizarDTBs, intToString(emisor));
	signalMutex(&mutexCpusAFinalizarDTBs);
	if(tieneLaClave){
		if(dtb->id == 0){
			waitMutex(&mutexCpusAFinalizarDTBs);
			pasarDTBAExit(dictionary_get(cpusAFinalizarDTBs, intToString(emisor)));
			signalMutex(&mutexCpusAFinalizarDTBs);
		}else{
			pasarDTBAExitGuardandoNuevo(dtb);
		}
	}
}
