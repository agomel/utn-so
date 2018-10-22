#include "segmentacionPura.h"
#include "stdbool.h"

int idSegmento = 0;

bool compararElementos(ElementoTablaSegPura elem1, ElementoTablaSegPura elem2){
	return elem1.base > elem2.base;
}

respuestaDeCargaEnMemoria guardarDatosSegPura(char* datos){
	//COMO SE SEGMENTA?? POR AHORA SE GUARDA TODO LO QUE LLEGÓ EN UN SEGMENTO

	log_debug(logger, "Guardando en paginacion pura");
	respuestaDeCargaEnMemoria respuesta;
	int tamanioSegmento = strlen(datos) + 1;
	ElementoTablaSegPura* elementoTabla = malloc(sizeof(ElementoTablaSegPura));

	elementoTabla->id = idSegmento;
	elementoTabla->base = offset;
	elementoTabla->limite = tamanioSegmento;

	list_add(tablaDeSegmentos, elementoTabla);

	t_list* idsSegmento = list_create();
	list_add(idsSegmento, idSegmento);

	memcpy(storage, datos, tamanioSegmento);

	offset = offset + tamanioSegmento;

	idSegmento++;

	respuesta.listaDeDirecciones = idsSegmento;
	respuesta.pudoGuardarlo = 0;

	return respuesta;
}

respuestaDeObtencionDeMemoria* obtenerDatosSegPura(t_list* posiciones){
	respuestaDeObtencionDeMemoria* respuesta;
	respuesta->cantidadDeLineas = 0;
	t_list* lista = list_create();
	respuesta->datos = lista;
	respuesta->pudoGuardarlo = 0;
	return respuesta;
}
