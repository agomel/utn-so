#include "consola.h"

void consola(){

	//escuchar la consola
	while(1){
		char* mensaje = readline("");
		char* comando =  strtok(mensaje, " ");
		char* parametro =  strtok(NULL, " ");
		int idDTB = atoi(parametro);

		if(strcmp(comando, "dump") == 0){
			if(strcmp(modo, "SEG_PURA") == 0)
				dumpSegPura(idDTB);

			if(strcmp(modo, "SEG_PAG") == 0)
				dumpSegPag(idDTB);

			if(strcmp(modo, "INV") == 0)
				dumpInvertida(idDTB);

		}else if(strcmp(comando, "exit") == 0){
			exit(200);
		}else{
			printf("Comando incorrecto\n");
		}
	}
}

void dumpSegPura(int idDTB){
	ElementoTablaProcesos* proceso = obtenerProcesoPorIdDTB(idDTB);
	int cantidadDeSegmentos = proceso->tablaSegmentos->elements_count;
	log_info(logger, "El DTB con id %d, tiene %d archivos abiertos en memoria", idDTB, cantidadDeSegmentos);
	for (int i = 0; i < cantidadDeSegmentos; ++i) {
		ElementoTablaSegPura* segmento = list_get(proceso->tablaSegmentos, i);
		respuestaDeObtencionDeMemoria* respuesta = obtenerDatosSegPura(idDTB, segmento->nombreArchivo);
		log_info(logger, "El archivo %d tiene estos datos guardados: %s", (i+1), respuesta->datos);
		freeRespuestaObtencion(respuesta);
	}
}

void dumpSegPag(idDTB){

}

void dumpInvertida(idDTB){

}
