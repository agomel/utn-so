#include "paginasInvertidas.h"

static void cargarTabla();
static int obtenerMarcoLibreInvertida();

void inicializarInvertida(t_config* configuracion){
	tablaPaginasInvertidas = list_create();
	idPagina = 0;
	idMarco = 0;
	tamanioPagina = config_get_int_value(configuracion, "TAM_PAGINA");
	cantidadMarcosTotales = tamanioMemoria / tamanioPagina;
}



int guardarDatosInvertida(int idDTB, char* datos, char* nombreArchivo){
	return 0;
}

respuestaDeObtencionDeMemoria* obtenerDatosInvertida(char* ids){
	respuestaDeObtencionDeMemoria* respuesta;
	return respuesta;
}

static RespuestaCargaSegPag* guardarDatosInternaSegPag(char* datos, char* nombreArchivo){
	log_debug(logger, "Guardando en segmentacion paginada");

	RespuestaCargaSegPag* respuesta = malloc(sizeof(RespuestaCargaSegPag));
	respuesta->resultado = 1;
	int totalLineas = cantidadDeLineas(datos);
	char** lineas = string_split(datos, "\n");
	int tamanioSegmento = totalLineas * tamanioLinea;
	int cantidadPaginas = 1;
	int lineasEnLaUltimaPagina = totalLineas % tamanioPagina;
	if(totalLineas > tamanioPagina){
		cantidadPaginas = totalLineas / tamanioPagina;
		if(lineasEnLaUltimaPagina != 0)
				cantidadPaginas++;
	}

	if((cantidadMarcosTotales - tablaDePaginas->elements_count) >= cantidadPaginas){
		respuesta->resultado = 0; //No hay error
		ElementoTablaSegPag* elementoSegmento = crearElemTablaSegPag(nombreArchivo, totalLineas);
		int lineaACargar = 0;
		for(int i = 0; i < cantidadPaginas; i++){
			char* textoAGuardar;
			int posicionMarco = obtenerMarcoLibre();
			if(posicionMarco == -1)
				respuesta->resultado = -1; //	BUSCAR CODIGO ERROR CUANDO NO HAY MARCOS LIBRES

			ElementoTablaPag* elementoPagina = malloc(sizeof(ElementoTablaPag));
			elementoPagina->idPag = idPagina;
			idPagina++;
			elementoPagina->marco = posicionMarco;
			list_add(elementoSegmento->paginas, elementoPagina);
			list_add(tablaDePaginas, elementoPagina);
			int base = storage + posicionMarco * (tamanioPagina * tamanioLinea); //Porque el tamanioPagina esta en lineas
			if(cantidadPaginas - 1 == i){ //Es la ultima pagina
				for (int j = 0;  j < lineasEnLaUltimaPagina; j++) {
					if(lineas[lineaACargar]==NULL)
						lineas[lineaACargar] = string_new();
					string_append(&lineas[lineaACargar], "\n");
					char* textoAEscribir = malloc(tamanioLinea);
					memcpy(textoAEscribir, lineas[lineaACargar], strlen(lineas[lineaACargar]) + 1);
					memcpy(base + tamanioLinea * j, textoAEscribir, tamanioLinea); //Guardando de a una linea
					free(textoAEscribir);
					lineaACargar++;
				}
			}else{
				for(int j = 0; j < (tamanioPagina); j++){
					string_append(&lineas[lineaACargar], "\n");
					char* textoAEscribir = malloc(tamanioLinea);
					memcpy(textoAEscribir, lineas[lineaACargar], strlen(lineas[lineaACargar]) + 1);
					memcpy(base + tamanioLinea * j, textoAEscribir, tamanioLinea); //Guardando de a una linea
					free(textoAEscribir);
					lineaACargar++;
				}
			}
		}
		log_debug(logger, "Datos guardados");
		respuesta->elementoTabla = elementoSegmento;
		respuesta->pesoArchivo = cantidadPaginas * tamanioPagina;
	}

	freeLineas(lineas);
	return respuesta;
}

static int obtenerMarcoLibreInvertida(){

}

static ElementoTablaInvertida* crearElementoTablaInvertida(){
	ElementoTablaInvertida* elemento = malloc(sizeof(ElementoTablaInvertida));
	elemento->marco = idMarco;
	idMarco++;
	elemento->idDTB = NULL;
	elemento->nombreArchivo = NULL;
	elemento->pagina = -1;
}

static void cargarTabla(){
	for(int i=0; i < cantidadMarcosTotales; i++){
		ElementoTablaInvertida* elemento = crearElementoTablaInvertida();
		list_add(tablaPaginasInvertidas, elemento);
	}
}
