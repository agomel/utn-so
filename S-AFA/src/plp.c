#include "plp.h"

void planificadorALargoPlazo() {
	int a = 1;
	while (a) {
		waitSem(&semCantidadEnNew);
		log_info(logger, "Hay procesos en la cola new");

		DTB* dtb = obtenerPrimerDTBEnNew();

		log_info(logger, "Cargando Dummy para dtb con scriptorio %s", dtb->escriptorio);

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
	dtbDummy->direccionesArchivos = dictionary_create();
	dtbDummy->estado = READY;
	agregarDTBALista(dtbDummy);
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
	if(dtb->estado == NEW){
		finalizarHistorialDeListaNew(idDTB);
	}
	cambiarEstado(idDTB, READY);
	signalSem(&cantidadTotalREADY);
}

void pasarDTBAExit(int idDTB){
	DTB* dtb = obtenerDTBDeCola(idDTB);
	if(!dtb){
		log_error(logger,"NO existe ese dtb");
	}else if(dtb->estado == EXIT){
		log_info(logger, "El DTB: %d ya se encontraba en EXIT", idDTB);
	}else{
		cambiarEstado(idDTB, EXIT);
		finalizarHistorialDeListaExit(idDTB);
		signalSem(&gradoMultiprogramacion);
	}
}
void pasarDTBAExitGuardandoNuevo(DTB* dtb) {
	if(dtb->estado == EXIT){
			log_info(logger, "El DTB: %d ya se encontraba en EXIT", dtb->id);
	}else{
		cambiarEstadoGuardandoNuevoDTB(dtb, EXIT);
		finalizarHistorialDeListaExit(dtb->id);
		signalSem(&gradoMultiprogramacion);
	}
}

void manejarErrores(int idDTB, char* path, int error){
	pasarDTBAExit(idDTB);
	switch(error){
		case 10001:
			log_error(logger, "path inexistente de archivo %s", path);
			break;

		case 10002:
		case 20003:
			log_error(logger, "espacio insuficiente en FM9 para archivo %s", path);
			break;

		case 20001:
		case 30001:
		case 40001:
			log_error(logger, "El archivo no se encuentra abierto %s", path);
			break;

		case 20002:
		case 30002:
		case 40002:
			log_error(logger, "Fallo de segmento/memoria en path %s", path);
			break;
		case 30003:
			log_error(logger, "Espacio insuficiente en MDJ para archivo %s", path);
			break;
		case 30004:
			log_error(logger, "El archivo no existe en MDJ, fue borrado previamente MDJ path: %s", path);
			break;
		case 50001:
			log_error(logger, "Archivo ya existente: %s", path);
			break;
		case 50002:
			log_error(logger, "Espacio insuficiente en path: %s", path);
			break;
		case 60001:
			log_error(logger, "El archivo no existe en path: %s", path);
			break;
		default:
			log_error(logger, "No reconozco el error, pero te termino el dtb");
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
